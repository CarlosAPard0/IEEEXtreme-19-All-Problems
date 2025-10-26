import shlex
import re
from collections import defaultdict

lines = []
while True:
    try:
        line = input()
    except EOFError:
        break
    line = line.rstrip('\n')
    if line.strip():
        lines.append(line)

if not lines:
    quit()

idx = 0
row_count = None
try:
    row_count = int(lines[idx].strip())
    idx += 1
except ValueError:
    row_count = None

if idx >= len(lines):
    quit()

rule_line = lines[idx].strip()
idx += 1
rules = {}
if rule_line:
    for part in rule_line.split(','):
        part = part.strip()
        if not part:
            continue
        key, value = part.split('=')
        rules[key.strip()] = int(value.strip())

if idx >= len(lines):
    quit()

fields_line = lines[idx].strip()
idx += 1
fields = [f.strip() for f in fields_line.split(',')]
field_count = len(fields)

log_lines = lines[idx:]
if row_count is not None:
    log_lines = log_lines[:row_count]

def new_record():
    return {
        'agents': set(),
        'ips': set(),
        'pdf_count': 0,
        'sessions': set(),
        'max_crawl': 0,
        'current_seq_len': 0,
        'last_pdf': None
    }

records = defaultdict(new_record)
pdf_regex = re.compile(r'GET\s+/document/(\d+)\.pdf\s+HTTP/1\.1')

for line in log_lines:
    if not line.strip():
        continue
    tokens = shlex.split(line)
    if len(tokens) != field_count:
        continue
    data = {fields[i]: tokens[i] for i in range(field_count)}
    if data.get('HTTP Status') != '200':
        continue

    user_id = data.get('Id', '').strip()
    if not user_id or user_id == '-':
        continue

    date_field = data.get('Date', '')
    if not (date_field.startswith('[') and date_field.endswith(']')):
        continue
    day = date_field[1:-1].split(':', 1)[0]

    record = records[(user_id, day)]

    ua = data.get('User Agent', '').strip()
    if ua and ua != '-':
        record['agents'].add(ua)

    ip = data.get('Client IP', '').strip()
    if ip and ip != '-':
        record['ips'].add(ip)

    session = data.get('Session Cookie', '').strip()
    if session and session != '-':
        record['sessions'].add(session)

    request = data.get('Request', '')
    match = pdf_regex.fullmatch(request)
    if match:
        record['pdf_count'] += 1
        pdf_num = int(match.group(1))
        last_pdf = record['last_pdf']
        if last_pdf is not None and pdf_num == last_pdf + 1:
            record['current_seq_len'] += 1
        else:
            record['current_seq_len'] = 1
        record['last_pdf'] = pdf_num
        record['max_crawl'] = max(record['max_crawl'], record['current_seq_len'])
    else:
        # mantiene current_seq_len sin cambiar
        record['current_seq_len'] = record['current_seq_len']

violations = []
for (user_id, _), record in records.items():
    if 'agent' in rules:
        count = len(record['agents'])
        if count >= rules['agent']:
            violations.append((user_id, 'agent', count))
    if 'ip' in rules:
        count = len(record['ips'])
        if count >= rules['ip']:
            violations.append((user_id, 'ip', count))
    if 'pdf' in rules:
        count = record['pdf_count']
        if count >= rules['pdf']:
            violations.append((user_id, 'pdf', count))
    if 'session' in rules:
        count = len(record['sessions'])
        if count >= rules['session']:
            violations.append((user_id, 'session', count))
    if 'crawl' in rules:
        count = record['max_crawl']
        if count >= rules['crawl']:
            violations.append((user_id, 'crawl', count))

violations.sort(key=lambda item: (item[0], item[1]))

if not violations:
    print("N/A")
else:
    for user_id, abuse_type, value in violations:
        print(f"{user_id} {abuse_type}={value}")
