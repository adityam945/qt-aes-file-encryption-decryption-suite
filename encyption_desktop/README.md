# List of API endpoints exposed by the Encryption Application Server (running on port 8080 by default).

## Please note: 
    No rate limiting or token auth is available for any endpoints


## You can test these using curl or Postman.

## 1. Register a Decryption Machine
Used by the client Python script to register its identity and public key.

    Endpoint: POST /register_machine

    Payload:

    JSON

    {
    "machine_id": "unique_hash_id",
    "machine_name": "Aditya-Laptop",
    "public_key": "hex_encoded_public_key"
    }
    Curl Example:

    Bash

    curl -X POST http://localhost:8080/register_machine \
        -H "Content-Type: application/json" \
        -d '{
            "machine_id": "abc12345",
            "machine_name": "DELL-WORKSTATION",
            "public_key": "5dc68dc53e1f44..."
            }'

## 2. Encrypt a Folder (AES)
Triggers the server to encrypt a local directory.

    Endpoint: POST /encrypt

    Payload:

    JSON

    {
    "source_dir": "/absolute/path/to/source",
    "output_dir": "/absolute/path/to/output"
    }
    Curl Example:

    Bash

    curl -X POST http://localhost:8080/encrypt \
        -H "Content-Type: application/json" \
        -d '{
            "source_dir": "/home/aditya/sql_data",
            "output_dir": "/home/aditya/backups"
            }'

## 3. RSA Wrap Session Key
Locks an existing AES session key for a specific registered machine and prepares the distribution folder.

    Endpoint: POST /rsa_wrap_key

    Payload:

    JSON

    {
    "key_path": "/path/to/encryption_timestamp/session_key.txt",
    "machine_name": "DELL-WORKSTATION" 
    }
    (Note: You can use machine_id instead of machine_name if preferred)

    Curl Example:

    Bash

    curl -X POST http://localhost:8080/rsa_wrap_key \
        -H "Content-Type: application/json" \
        -d '{
            "key_path": "/home/aditya/backups/encryption_20250101/session_key.txt",
            "machine_name": "DELL-WORKSTATION"
            }'

## 4. Fetch Audit Records
Retrieves a JSON dump of all registered machines and encrypted folder history.

    Endpoint: GET /records

    Curl Example:

    Bash

    curl http://localhost:8080/records

## 5. Show Server GUI
If the server is running in "Headless Mode" (background), this triggers the Qt Window to appear on the host screen.

    Endpoint: POST /show_ui (or GET /show_ui)

    Curl Example:

    Bash

    curl -X POST http://localhost:8080/show_ui