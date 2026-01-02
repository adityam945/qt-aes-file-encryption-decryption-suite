# standalone python script to register client machine
# written byt Gemini 3
import hashlib
import subprocess
import sys
import argparse
import json
import urllib.request
import urllib.error
import platform  # <--- Needed for Hostname
from nacl.public import PrivateKey

def get_machine_id():
    """Generates a stable unique hash based on hardware UUID."""
    try:
        if sys.platform == "win32":
            cmd = "wmic csproduct get uuid"
            uuid = subprocess.check_output(cmd, shell=True).decode().split()[1]
        else:
            cmd = "cat /etc/machine-id 2>/dev/null || cat /var/lib/dbus/machine-id"
            try:
                uuid = subprocess.check_output(cmd, shell=True).decode().strip()
            except:
                import uuid as _uuid
                uuid = str(_uuid.getnode())
        return hashlib.sha256(uuid.encode()).hexdigest()
    except Exception as e:
        return "UNKNOWN_ID"

def get_machine_name():
    """Returns the hostname of the computer."""
    return platform.node()

def register_to_server(base_url, machine_id, machine_name, public_key):
    endpoint = f"{base_url}/register_machine"
    
    # --- UPDATED PAYLOAD ---
    payload = {
        "machine_id": machine_id,
        "machine_name": machine_name,  # <--- Sending Name
        "public_key": public_key
    }
    
    data = json.dumps(payload).encode('utf-8')
    req = urllib.request.Request(endpoint, data=data, method='POST')
    req.add_header('Content-Type', 'application/json')

    try:
        print(f"[>] Sending registration for '{machine_name}'...")
        with urllib.request.urlopen(req) as response:
            print(f"[<] Response: {response.read().decode('utf-8')}")
    except urllib.error.URLError as e:
        print(f"[!] Error: {e.reason}")

def generate_keys():
    private_key = PrivateKey.generate()
    with open("decryption_machine.private_key", "wb") as f:
        f.write(private_key.encode())
    return private_key.public_key.encode().hex()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--url", default="http://localhost:8080")
    args = parser.parse_args()

    hwid = get_machine_id()
    name = get_machine_name()
    pub_key = generate_keys()

    print(f"[*] ID:   {hwid}")
    print(f"[*] Name: {name}")
    
    register_to_server(args.url, hwid, name, pub_key)