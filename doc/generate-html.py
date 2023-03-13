#!/usr/bin/python3
# Generate a HTML page from the .rst doc files

import docutils.core
import os
import re

additional_style = """
body {
    background-color: #181825;
    margin: 0;
}

.document {
    width: 50%;
    padding: 20px;
    margin: auto;
    margin-top: 0;
    margin-bottom: 0;
    background-color: #1e1e2e;
    color: #cdd6f4;
    box-shadow: 5px 5px 10px #11111b;
}

.section {
    margin-top: 2em;
}

.section h1 {
    font-family: monospace !important;
    font-size: 1.5em !important;
    text-decoration: underline dotted #7f849c;
}

a {
    font-family: monospace;
    color: #89dceb;
    text-decoration: underline #9399b2;
}

a:visited {
    color: #89dceb;
    text-decoration: underline #9399b2;
}

a:hover {
    color: #b4befe;
    text-decoration: underline dotted #9399b2;
}
"""


def list_dir_recursive(dir: str) -> list:
    files = os.listdir(dir)
    for f in files:
        if os.path.isdir(f):
            files.extend([f + '/' + x for x in list_dir_recursive(f)])
    return files


files = sorted(list_dir_recursive('.'))
links = []

if not os.path.exists('html'):
    os.mkdir('html')

for doc in files:
    if not doc.endswith('.rst'):
        continue
    if doc.startswith('html'):
        continue

    basename = doc.rsplit('.', 1)[0]
    dest = f'html/{basename}.html'

    dest_dir = os.path.dirname(dest)
    if not os.path.exists(dest_dir):
        os.mkdir(dest_dir)

    print('Generating', dest)

    docutils.core.publish_file(
        source_path=f'{basename}.rst',
        destination_path=dest,
        writer_name='html'
    )

    # Add custom styles
    with open(dest) as f:
        html = f.read()

    tag = '<style type="text/css">'
    html = html.replace(tag, tag + additional_style)

    link_back = '<a href="../index.html">&lt;&lt;&lt; Back</a>'

    found = re.search(r'<h1 class="title">.*</h1>', html)
    if found:
        html = html.replace(found.group(0), found.group(0) + link_back)

    with open(dest, 'w') as f:
        f.write(html)

    links.append(f'<a href="html/{basename}.html">{doc}</a><br>')


links = '\n'.join(links)
index_source = f"""
<html>
    <head>
        <title>mcc documentation</title>
        <style>

        a {{
            font-family: monospace;
            color: #89dceb;
            text-decoration: underline #9399b2;
        }}

        a:visited {{
            color: #89dceb;
            text-decoration: underline #9399b2;
        }}

        a:hover {{
            color: #b4befe;
            text-decoration: underline dotted #9399b2;
        }}

        body {{
            background-color: #181825;
            margin: 0;
        }}

        .document {{
            width: 50%;
            margin: auto;
            padding: 20px;
            background-color: #1e1e2e;
            color: #cdd6f4;
            box-shadow: 5px 5px 10px #11111b;
        }}

        </style>
    </head>
    <body>
    <div class="document">
        <h1>mcc</h1>
        <p>
            The official mocha compiler documentation.
        </p>
        <hr>
        <p>
            Here is the list of all generated HTML pages from the .rst files:
        </p>
{links}
    </div>
    </body>
</html>
"""

with open('index.html', 'w') as f:
    f.write(index_source)
