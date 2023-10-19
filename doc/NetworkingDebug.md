# Windows

```powershell
Test-NetConnection -ComputerName 127.0.0.1 -Port 8080
```
```powershell
netstat -ano | findstr "LISTENING"
```
```powershell
# The next powershell program send a message to a TCP socket
$tcpClient = New-Object System.Net.Sockets.TcpClient("127.0.0.1", 8080)
$stream = $tcpClient.GetStream()
$writer = New-Object System.IO.StreamWriter($stream)
$writer.WriteLine("Tu mensaje aquí")
$writer.Flush()
```

*Note: ncat is part of Nmap, not included by default (https://nmap.org/download.html#windows)*
```powershell
echo "1 2 3 4 5 6 7 8 9 0 10 11 12 13 13" | ncat 127.0.0.1 8080
```

Send file (as a bytes) to 127.0.0.1:8080
```powershell
Get-Content .\a.bin -Raw | ncat 127.0.0.1 8080
```


# Unix

```bash
netstat -tuln | grep LISTEN
```
Send plain text to 127.0.0.1:8080
```bash
echo "hello world" | nc 127.0.0.1 8080
```
Send bytes to 127.0.0.1:8080
```bash
echo -e "\x48\x65\x6c\x6c\x6f" | nc 127.0.0.1 8080
```
Send file (as a bytes) to 127.0.0.1:8080
```bash
cat data.bin | nc 127.0.0.1 8080
```
