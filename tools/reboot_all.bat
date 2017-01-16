FOR /F %%D IN (device.list) DO (
	upload.py -host localhost -port 1883 -user ESP -pwd 1234 -id %%D -r)