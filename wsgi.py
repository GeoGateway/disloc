
"""
	wsgi.py
		-- run as flask wrapper as wsgi application
		-- run as non-root user: ~/miniconda3/bin/mod_wsgi-express start-server wsgi.py &
	
	Installation:
		-- need apache2 apache2-dev
		-- pip install mod_wsgi
"""

from disloc_service_API import app as application
