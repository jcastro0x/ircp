# SSL
Commands in OpenSSL for generating the required keys and certificates.

> Note
>
> In the project server's `binaries` folder, the default files are included,
> please remember to replace them with your own versions in non-development environments.

## Certificate Authority
### Private Key of the Certificate Authority (CA)
```bash
openssl genpkey -algorithm RSA -out ca-key.pem

#print human readable info
openssl rsa -in ca-key.pem -text -noout
```
### Public Certificate of the Certificate Authority (CA)
Lo que leeran los clientes. Contiene la clave publica del ca-key e información de la entidad.
```bash
openssl req -new -x509 -key ca-key.pem -out ca.pem -days 365

#print human readable info
openssl x509 -in ca.pem -text -noout
```

___

## Server
### Private Key of the Server
```bash
openssl genpkey -algorithm RSA -out server-key.pem

#print human readable info
openssl rsa -in server-key.pem -text -noout
```
### Certificate Signing Request (CSR)
```bash
openssl req -new -key server-key.pem -out server.csr

#print human readable info
openssl req -in server.csr -text -noout
```
### Signing the CSR with our CA
```bash
openssl x509 -req -in server.csr -CA ca.pem -CAkey ca-key.pem -CAcreateserial -out server.pem -days 365

#print human readable info
openssl x509 -in server.pem -text -noout 
```
### Creation of Diffie-Hellman Key
```bash
openssl dhparam -out dh4096.pem 4096

#print human readable info
openssl dhparam -in dh4096.pem -text
```

### Combine Private key and Certificate
By default, these commands do not generate a server.pem file, which contains all essential information required by
Boost.SSL. Therefore, it is necessary to manually create this file.
```bash
cat server-key.pem server.pem > combined.pem
```
___


## Without CA
This quicker option is useful for the server to load private keys and certificates in a development environment without 
dealing with the CA.
```bash
openssl req -x509 -newkey rsa:4096 -keyout server.pem -out server.pem -days 365
openssl dhparam -out dh4096.pem 4096
```
