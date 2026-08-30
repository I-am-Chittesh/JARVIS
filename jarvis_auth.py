import os
import platform
import pyautogui
from flask import Flask, request, jsonify

app = Flask(__name__)

# Windows System Hosts File (Requires Admin Privileges)
HOSTS_PATH = r"C:\Windows\System32\drivers\etc\hosts"
REDIRECT_IP = "127.0.0.1"
BLOCKED_DOMAINS = ["www.youtube.com", "youtube.com", "www.instagram.com", "instagram.com", "x.com"]

# --- PATENT CLAIM 1: HARDWARE-ENFORCED REGULATION ---
@app.route('/api/focus/start', methods=['POST'])
def engage_focus_lock():
    try:
        with open(HOSTS_PATH, 'r+') as file:
            content = file.read()
            for domain in BLOCKED_DOMAINS:
                if domain not in content:
                    file.write(f"{REDIRECT_IP} {domain}\n")
        print("\n[!] FOCUS LOCKED: Network access to distracting domains severed.")
        return jsonify({"status": "locked"}), 200
    except PermissionError:
        print("\n[ERROR] Daemon must be run as Administrator to enforce network rules.")
        return jsonify({"status": "permission_denied"}), 403

@app.route('/api/focus/stop', methods=['POST'])
def disengage_focus_lock():
    try:
        with open(HOSTS_PATH, 'r+') as file:
            lines = file.readlines()
            file.seek(0)
            for line in lines:
                if not any(domain in line for domain in BLOCKED_DOMAINS):
                    file.write(line)
            file.truncate()
        print("\n[>] FOCUS RELEASED: Network access restored.")
        return jsonify({"status": "unlocked"}), 200
    except PermissionError:
        return jsonify({"status": "permission_denied"}), 403

# --- PATENT CLAIM 2: DYNAMIC HARDWARE REMAPPING (MEDIA) ---
@app.route('/api/media/sync', methods=['POST'])
def media_sync():
    data = request.json
    action = data.get("action")
    
    if action == "next":
        pyautogui.press('nexttrack')
        print("[>] MEDIA: Next Track")
    elif action == "prev":
        pyautogui.press('prevtrack')
        print("[>] MEDIA: Previous Track")
    elif action == "vol_up":
        pyautogui.press('volumeup')
        print("[>] MEDIA: Volume +")
    elif action == "vol_down":
        pyautogui.press('volumedown')
        print("[>] MEDIA: Volume -")
        
    return jsonify({"status": "success", "action": action}), 200

if __name__ == '__main__':
    print("===================================================")
    print(" J.A.R.V.I.S. DAEMON ACTIVE")
    print(" Awaiting hardware interrupts...")
    print("===================================================")
    app.run(host='0.0.0.0', port=5000)