import kdcproxy
import os
import ssl
import sys
from wsgiref.simple_server import make_server

if len(sys.argv) > 1:
    port = int(sys.argv[1])
else:
    port = 8443
if len(sys.argv) > 2:
    pem = sys.argv[2]
else:
    pem = '*'

server = make_server('localhost', port, kdcproxy.Application())

# Configure a secure SSLContext that requires TLS 1.2 or higher.
    context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
    # Prefer using minimum_version when available (Python 3.7+).
    tls_version = getattr(ssl, "TLSVersion", None)
    if tls_version is not None and hasattr(context, "minimum_version"):
        context.minimum_version = tls_version.TLSv1_2
    else:
        # Fallback for older Python versions: use a TLSv1.2-only protocol.
        context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)

	server.socket = context.wrap_socket(server.socket, certfile=pem, server_side=True)

os.write(sys.stdout.fileno(), b'proxy server ready\n')
server.serve_forever()
