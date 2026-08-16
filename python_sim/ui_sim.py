import pygame
import sys
import time

# --- INITIALIZATION ---
pygame.init()
WIDTH, HEIGHT = 480, 640
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("J.A.R.V.I.S. UI Simulator - Patent PoW")

# --- COLORS (High-Contrast Cyberpunk) ---
BLACK = (10, 10, 10)
CYAN = (0, 255, 204)
GREEN = (57, 255, 20)
WHITE = (240, 240, 240)
YELLOW = (255, 204, 0)
DARK_GREY = (50, 50, 50)

# --- FONTS ---
# Using default system fonts for plug-and-play capability
font_large = pygame.font.SysFont("consolas", 64, bold=True)
font_medium = pygame.font.SysFont("consolas", 32, bold=True)
font_small = pygame.font.SysFont("consolas", 24)

# --- FSM STATES ---
STATE_BOOT = 0
STATE_MENU = 1
STATE_TIMER = 2
STATE_CAMERA = 3
STATE_MEDIA = 4

current_state = STATE_BOOT
state_start_time = time.time()

# --- GLOBAL VARIABLES ---
menu_options = ["Focus Timer", "Optical Log", "Media Sync"]
menu_idx = 0

timer_options = ["Work (25:00)", "Rest (05:00)"]
timer_idx = 0
is_timer_running = False
timer_duration = 0
timer_start_ticks = 0

camera_flash = False
camera_flash_time = 0
camera_status = "CAMERA READY"

media_vol = 50
media_toast = ""
media_toast_time = 0

# --- INPUT TRACKING (For Long Press) ---
b_key_down_time = 0
b_key_is_down = False

# --- HELPER FUNCTIONS ---
def draw_text(surface, text, font, color, x, y, center=False):
    text_obj = font.render(text, True, color)
    text_rect = text_obj.get_rect()
    if center:
        text_rect.center = (x, y)
    else:
        text_rect.topleft = (x, y)
    surface.blit(text_obj, text_rect)

# --- MAIN LOOP ---
clock = pygame.time.Clock()

while True:
    screen.fill(BLACK)
    current_time = time.time()
    
    # 1. EVENT HANDLING (Keyboard as Hardware)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()
            
        if event.type == pygame.KEYDOWN:
            # LONG PRESS TRACKING (Button B)
            if event.key == pygame.K_b:
                b_key_is_down = True
                b_key_down_time = current_time

            # APP SPECIFIC INPUTS
            if current_state == STATE_MENU:
                if event.key == pygame.K_a: # Button A (Up)
                    menu_idx = (menu_idx - 1) % 3
                elif event.key == pygame.K_b: # Button B (Down)
                    menu_idx = (menu_idx + 1) % 3
                elif event.key == pygame.K_RETURN: # Encoder Press
                    if menu_idx == 0: current_state = STATE_TIMER
                    elif menu_idx == 1: current_state = STATE_CAMERA
                    elif menu_idx == 2: current_state = STATE_MEDIA

            elif current_state == STATE_TIMER:
                if not is_timer_running:
                    if event.key in [pygame.K_a, pygame.K_b, pygame.K_UP, pygame.K_DOWN]:
                        timer_idx = (timer_idx + 1) % 2
                    elif event.key == pygame.K_RETURN:
                        is_timer_running = True
                        timer_duration = 25 * 60 if timer_idx == 0 else 5 * 60
                        timer_start_ticks = current_time

            elif current_state == STATE_CAMERA:
                if event.key == pygame.K_RETURN or event.key == pygame.K_a:
                    camera_flash = True
                    camera_flash_time = current_time
                    camera_status = "SYNCING..."

            elif current_state == STATE_MEDIA:
                if event.key == pygame.K_UP: media_vol = min(100, media_vol + 5)
                elif event.key == pygame.K_DOWN: media_vol = max(0, media_vol - 5)
                elif event.key == pygame.K_a:
                    media_toast = "-> NEXT TRACK"
                    media_toast_time = current_time
                elif event.key == pygame.K_b:
                    media_toast = "<- PREV TRACK"
                    media_toast_time = current_time

        if event.type == pygame.KEYUP:
            if event.key == pygame.K_b:
                b_key_is_down = False

    # 2. OVERRIDE LOGIC (Hold B for 1.5s)
    if b_key_is_down and (current_time - b_key_down_time > 1.5):
        if current_state not in [STATE_BOOT, STATE_MENU]:
            current_state = STATE_MENU
            is_timer_running = False
            b_key_is_down = False # Reset trigger

    # 3. DRAWING FSM STATES
    if current_state == STATE_BOOT:
        draw_text(screen, "J.A.R.V.I.S.", font_large, CYAN, WIDTH//2, HEIGHT//2 - 20, center=True)
        draw_text(screen, "CYBER-PHYSICAL GATEWAY", font_small, WHITE, WIDTH//2, HEIGHT//2 + 30, center=True)
        if current_time - state_start_time > 1.5:
            current_state = STATE_MENU

    elif current_state == STATE_MENU:
        draw_text(screen, "MAIN MENU", font_medium, WHITE, 20, 30)
        pygame.draw.line(screen, DARK_GREY, (20, 70), (460, 70), 2)
        
        for i, opt in enumerate(menu_options):
            y_pos = 120 + (i * 60)
            if i == menu_idx:
                draw_text(screen, f"> {opt}", font_medium, GREEN, 40, y_pos)
            else:
                draw_text(screen, f"  {opt}", font_medium, WHITE, 40, y_pos)

    elif current_state == STATE_TIMER:
        if not is_timer_running:
            draw_text(screen, "SELECT TIMER", font_medium, WHITE, 20, 30)
            pygame.draw.line(screen, DARK_GREY, (20, 70), (460, 70), 2)
            
            for i, opt in enumerate(timer_options):
                y_pos = 140 + (i * 60)
                if i == timer_idx:
                    draw_text(screen, f"> {opt}", font_medium, GREEN, 40, y_pos)
                else:
                    draw_text(screen, f"  {opt}", font_medium, WHITE, 40, y_pos)
        else:
            elapsed = current_time - timer_start_ticks
            rem = max(0, int(timer_duration - elapsed))
            mins, secs = divmod(rem, 60)
            
            title = "FOCUS: WORK" if timer_idx == 0 else "FOCUS: REST"
            draw_text(screen, title, font_medium, WHITE, WIDTH//2, 100, center=True)
            draw_text(screen, f"{mins:02d}:{secs:02d}", font_large, CYAN, WIDTH//2, HEIGHT//2, center=True)

    elif current_state == STATE_CAMERA:
        if camera_flash:
            screen.fill(WHITE)
            if current_time - camera_flash_time > 0.1:
                camera_flash = False
        else:
            pygame.draw.rect(screen, WHITE, (40, 100, 400, 320), 2)
            draw_text(screen, camera_status, font_medium, YELLOW, WIDTH//2, 50, center=True)
            if camera_status == "SYNCING..." and (current_time - camera_flash_time > 1.5):
                camera_status = "CAMERA READY"
            
            draw_text(screen, "PRESS ENCODER TO SHUTTER", font_small, WHITE, WIDTH//2, 470, center=True)

    elif current_state == STATE_MEDIA:
        draw_text(screen, "NOW PLAYING", font_medium, GREEN, 20, 30)
        pygame.draw.line(screen, DARK_GREY, (20, 70), (460, 70), 2)
        draw_text(screen, "Song: Unknown Artist", font_small, WHITE, 20, 100)
        
        if current_time - media_toast_time < 0.5:
            draw_text(screen, media_toast, font_medium, YELLOW, 20, 160)
            
        draw_text(screen, f"VOL: {media_vol}%", font_medium, CYAN, 20, 400)
        pygame.draw.rect(screen, WHITE, (20, 450, 440, 30), 2)
        pygame.draw.rect(screen, CYAN, (22, 452, int(436 * (media_vol/100)), 26))

    pygame.display.flip()
    clock.tick(60) # 60 FPS for buttery smooth rendering