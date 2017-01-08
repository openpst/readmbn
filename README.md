## readmbn
`readmbn` is a CLI tool for reading and display information about files in Qualcomm mbn format binaries.

### Compiling

    git clone https://github.com/openpst/readmbn --recursive
    make

### Usage
To display simple information about the mbn file, execute:

    readmbn /path/to/file.mbn

You can extract the code, signature, or X509 segments of the mbn file with:

    $ readmbn /path/to/file.mbn -e code -o code.bin
    $ readmbn /path/to/file.mbn -e signature -o signature.hash
    $ readmbn /path/to/file.mbn -e x509 -o x509.der

Of course, le standard help with 
    $ readmbn --help

X509 certificates are in DER format and can be parsed with OpenSSL

    openssl x509 -in x509.der -inform der -text -noout

