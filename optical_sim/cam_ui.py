import os
import json
import time
import tkinter as tk
from PIL import Image, ImageTk

def run_optical_ui(image_name):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    image_path = os.path.join(script_dir, image_name)
    processed_img_path = os.path.join(script_dir, "optimized_capture.png")
    db_path = os.path.join(script_dir, "local_database.json")

    if not os.path.exists(image_path):
        print(f"[ERROR] Cannot find {image_path}")
        return

    # 1. Process the Image specifically for the ILI9341 TFT Screen
    with Image.open(image_path) as img:
        width, height = img.size
        original_kb = round(os.path.getsize(image_path) / 1024, 2)
        
        # Convert to Grayscale and resize to fit the 240x320 hardware constraint
        optimized_img = img.convert("L") 
        optimized_img.thumbnail((220, 260)) # Slightly smaller than 240x320 to fit the borders
        optimized_img.save(processed_img_path, format="PNG", optimize=True)
        new_kb = round(os.path.getsize(processed_img_path) / 1024, 2)
        
        # Load the newly optimized image to display on our simulator screen
        display_img = Image.open(processed_img_path)

    # 2. Build the Payload and Save to DB (Runs silently in the background)
    payload = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
        "device": "jarvis_edge_node_01",
        "action": "optical_shutter",
        "telemetry": {
            "original_resolution": f"{width}x{height}",
            "hardware_target": "ILI9341_TFT (240x320)",
            "original_kb": original_kb,
            "optimized_kb": new_kb
        },
        "status": "LOCAL_SYNC_SUCCESS"
    }

    with open(db_path, "a") as db_file:
        db_file.write(json.dumps(payload) + "\n")

    print("[>] SYNC COMPLETE: Telemetry written to local_database.json")

    # 3. Launch the JARVIS Hardware Simulator Window
    root = tk.Tk()
    root.title("JARVIS Edge UI")
    
    # Lock the window to the exact physical TFT display size
    root.geometry("240x320")
    root.configure(bg="#000000")
    root.resizable(False, False) # Prevent resizing to prove hardware constraints

    # Top Status Bar
    header = tk.Label(root, text="SYS.CAP // OPTICAL", fg="#00ff00", bg="#000000", font=("Courier", 10, "bold"))
    header.pack(pady=(10, 5))

    # Main Image Display
    tk_img = ImageTk.PhotoImage(display_img)
    img_label = tk.Label(root, image=tk_img, bg="#000000", bd=1, relief="solid")
    img_label.image = tk_img
    img_label.pack()

    # Bottom Sync Status
    footer = tk.Label(root, text="DB SYNC: SUCCESS", fg="#00ffff", bg="#000000", font=("Courier", 9, "bold"))
    footer.pack(side="bottom", pady=(0, 15))

    root.mainloop()

if __name__ == "__main__":
    run_optical_ui("test_capture.png")