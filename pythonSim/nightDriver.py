import pygame, sys, math, random

pygame.init()
WIDTH, HEIGHT = 480, 320
win = pygame.display.set_mode((WIDTH, HEIGHT))
clock = pygame.time.Clock()

# --- Parameters ---
NUM_SEGMENTS = 40
POST_SPACING = 8
ROAD_MIN_WIDTH = 40
ROAD_MAX_WIDTH = 200
GEAR_SPEEDS = [0, 2.5, 4.5, 6.5, 9.0]
ACCEL = 0.15
FRICTION = 0.05
TURN_RATE = 0.05
CENTER_SCREEN = WIDTH // 2
HORIZON = HEIGHT // 2
BACKGROUNG_WRAPAROUND = WIDTH * 2

# --- State ---
gear = 1
speed = 0.0
player_x = 0.0
road_curve = 0.0
curve_target = 0.0
curve_timer = 0
divider_offset = 0
total_x = 0

# buffer of road x-offsets (bottom → top)
road_x = [0.0 for _ in range(NUM_SEGMENTS)]

# --- Colors ---
BLACK = (0,0,0)
WHITE = (255,255,255)
RED = (255,80,80)
SKY_BLUE = (64,168,229)
GRASS_GREEN = (0, 179, 54)
CAR_COLOR = (255,140,0)
COLOR_YELLOW = (255, 215, 0)
MOUNTAIN_GRAY = (119, 119, 119)
font = pygame.font.SysFont(None, 24)

# Add near the top of your code
COLOR_RED = (255, 60, 60)
COLOR_WHITE = (255, 255, 255)
COLOR_ALT_FLAG = [0]  # mutable container to remember alternation

def draw_car():
    cx = CENTER_SCREEN
    pygame.draw.polygon(win, CAR_COLOR, [
        (cx-60, HEIGHT),
        (cx+60, HEIGHT),
        (cx+50, HEIGHT-40),
        (cx-50, HEIGHT-40)
    ])

def display_text(offroad):
    info = f"Gear:{gear}  Speed:{speed:.1f}  Player X:{player_x:.3f}"
    if offroad:
        info += "   OFF ROAD!"
    t = font.render(info, True, RED if offroad else WHITE)
    win.blit(t, (10,10))

def update_road_buffer():
    """Advance road one segment forward and re-normalize the buffer."""
    global player_x
    global total_x
    
    delta = road_x[1] - road_x[0]
    # shift buffer
    for i in range(NUM_SEGMENTS - 1):
        road_x[i] = road_x[i + 1]
    # renormalize all entries
    for i in range(NUM_SEGMENTS):
        road_x[i] -= delta
    player_x -= delta * 3
    # generate new top
    road_x[-1] = road_x[-2] + road_curve
    # flip color pattern each new row
    COLOR_ALT_FLAG[0] ^= 1

def draw_road():
    """Draw each pair of posts with alternating red/white pattern."""
    
    pygame.draw.rect(win, GRASS_GREEN, (0, HORIZON, WIDTH, (HEIGHT-HORIZON)))
    
    Y_OFFSET = int(HEIGHT * 0.10)
    for i in range(NUM_SEGMENTS):
        y = HEIGHT - i * POST_SPACING
        
        if (y > HORIZON - 1):
            # depth = (y - HORIZON) / (HEIGHT - HORIZON)
            depth = (y - HORIZON) / (HEIGHT - HORIZON)
            width = ROAD_MIN_WIDTH + depth * (ROAD_MAX_WIDTH - ROAD_MIN_WIDTH)
            # cx = CENTER_SCREEN + ((road_x[i] - player_x) * 150) * (1 + i/NUM_SEGMENTS)
            cx = CENTER_SCREEN + ((road_x[i] - player_x) * 120) * (2*(1-depth) + 1)

            # Choose alternating colors, flipping with COLOR_ALT_FLAG
            if (i + COLOR_ALT_FLAG[0]) % 2 == 0:
                left_color = COLOR_RED
                right_color = COLOR_RED
            else:
                left_color = COLOR_WHITE
                right_color = COLOR_WHITE

            pygame.draw.rect(win, BLACK, (cx - width, y, width * 2, 8))
            pygame.draw.rect(win, left_color,  (cx - width, y, 6, 8))
            pygame.draw.rect(win, right_color, (cx + width - 2, y, 6, 8))
            
            global divider_offset
            
            if((y + divider_offset) % 10 > 3):
                pygame.draw.rect(win, COLOR_YELLOW, (cx, y, 6, 8))
                
# --- Main loop ---
while True:
    for e in pygame.event.get():
        if e.type == pygame.QUIT:
            pygame.quit(); sys.exit()
        elif e.type == pygame.KEYDOWN:
            if e.key in [pygame.K_1, pygame.K_2, pygame.K_3, pygame.K_4]:
                gear = int(e.unicode)

    keys = pygame.key.get_pressed()

    # speed control
    target = GEAR_SPEEDS[gear]
    if keys[pygame.K_w]:
        speed += ACCEL
    else:
        speed -= FRICTION
    speed = max(0, min(speed, target))

    # steering
    steer = 0
    if keys[pygame.K_a]: steer = -1.0
    elif keys[pygame.K_d]: steer = 1.0
    player_x += steer * TURN_RATE * speed

    # random curvature
    curve_timer -= 1
    if curve_timer <= 0:
        curve_target = random.uniform(-0.05, 0.05)
        curve_timer = random.randint(60, 180)
    road_curve += (curve_target - road_curve)*0.05


    # move road forward proportionally to speed
    advance = speed/4.0
    scroll_accum = getattr(update_road_buffer, "accum", 0.0)
    scroll_accum += advance
    while scroll_accum >= 1.0:
        update_road_buffer()
        divider_offset += 1
        scroll_accum -= 1.0
    update_road_buffer.accum = scroll_accum
    divider_offset %= 10

    # off-road check
    offroad = abs(player_x) > 0.8
    if offroad: speed *= 0.97

    # draw
    win.fill(SKY_BLUE)
    draw_road()
    draw_car()
    display_text(offroad)
    pygame.display.flip()
    clock.tick(60)
