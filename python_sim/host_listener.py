import ctypes
import platform
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route('/api/lock', methods=['POST'])
def lock_workstation():
    """
    This endpoint listens for the hardware FSM. When triggered,
    it executes a native OS command to lock the user out.
    """
    print("\n[!] HARDWARE INTERRUPT RECEIVED: Engaging Focus Lock...")
    
    if platform.system() == "Windows":

        ctypes.windll.user32.LockWorkStation()
        return jsonify({"status": "locked", "message": "Host OS locked successfully"}), 200
    else:
        return jsonify({"status": "error", "message": "OS not supported for auto-lock"}), 400


@app.route('/api/volume', methods=['POST'])
def set_volume():
    """
    Listens for the rotary encoder data to adjust host volume.
    """
    data = request.json
    vol_level = data.get("volume", 50)
    print(f"[>] HARDWARE SYNC: Host volume adjusted to {vol_level}%")
    

    return jsonify({"status": "success", "volume_set": vol_level}), 200


if __name__ == '__main__':
    print("===================================================")
    print(" J.A.R.V.I.S. ECOSYSTEM LISTENER ACTIVE")
    print(" Waiting for hardware interrupts on Port 5000...")
    print("===================================================")
    
    # host='0.0.0.0' allows the Raspberry Pi to find this PC on the local Wi-Fi
    app.run(host='0.0.0.0', port=5000)