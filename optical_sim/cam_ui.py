import os
import json
import google.generativeai as genai
import PIL.Image
from dotenv import load_dotenv

# 1. Get the exact folder this script is living in (optical_sim)
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
# 2. Get the main JARVIS folder (one level up) to find the API key
ROOT_DIR = os.path.dirname(SCRIPT_DIR)

# Securely load the .env from the root folder
load_dotenv(os.path.join(ROOT_DIR, ".env"))
genai.configure(api_key=os.environ.get("GEMINI_API_KEY"))

def process_optical_log(image_name):
    # 3. Force Python to look in the optical_sim folder for the image
    image_path = os.path.join(SCRIPT_DIR, image_name)
    
    print(f"[!] HARDWARE TRIGGER: Optical Log Captured -> {image_path}")
    print("[*] SYNCING: Transmitting to Vision Engine...")
    
    try:
        img = PIL.Image.open(image_path)
    except FileNotFoundError:
        print(f"[ERROR] No image found at: {image_path}")
        return
        
    model = genai.GenerativeModel('gemini-1.5-flash')
    
    prompt = """
    You are a backend processing engine for a screenless hardware device.
    Analyze this image. 
    - If it is food, return a JSON object with 'type': 'food', 'calories': number, 'protein_g': number, 'carbs_g': number, 'fat_g': number.
    - If it is a whiteboard or document, return a JSON object with 'type': 'document', 'transcription': 'text here', 'summary': 'short summary'.
    Return ONLY raw JSON. Do not include markdown formatting like ```json.
    """
    
    response = model.generate_content([prompt, img])
    
    print("\n[>] SYNC COMPLETE. Database Entry Generated:")
    
    try:
        parsed_data = json.loads(response.text.strip())
        print(json.dumps(parsed_data, indent=4))
    except json.JSONDecodeError:
        print("Raw Output (JSON parsing failed):")
        print(response.text)

if __name__ == "__main__":
    process_optical_log("test_capture.png")