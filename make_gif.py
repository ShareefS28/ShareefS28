from PIL import Image, ImageDraw, ImageFont
import glob

FONT_PATH = "C:/Windows/Fonts/consola.ttf"
font = ImageFont.truetype(FONT_PATH, 20)

BG = (0, 0, 0, 0)
# BG = (0, 0, 0)
# FG = (255, 255, 255) # use in fill
COLS, ROWS = 80, 24
SHADES = ".,-~:;=!*#$@"

char_w = font.getlength("M")
ascent, descent = font.getmetrics()
line_h = ascent + descent

W = int(COLS * char_w) + 40
H = ROWS * line_h + 40

def color_for(ch):
    t = SHADES.index(ch) / (len(SHADES) - 1)   # 0 dark -> 1 bright
    t = t ** 1.6                               # curve: deepens shadows
    v = int(60 + 195 * t)                      # 40 (dim gray) -> 255 (white)
    return (v, v, v, 255)

def to_gif_frame(img):
    alpha = img.getchannel("A")
    p = img.convert("RGB").convert("P", palette=Image.ADAPTIVE, colors=255)
    mask = alpha.point(lambda a: 255 if a <= 128 else 0)
    p.paste(255, mask)             # index 255 = transparent
    return p

files = sorted(glob.glob("asset/frames/frame_*.txt"))
frames = []

for path in files:
    with open(path, "r") as f:
        lines = f.read().replace("\r", "").split("\n")[:ROWS]

    img = Image.new("RGBA", (W, H), BG)
    draw = ImageDraw.Draw(img)
    for i, line in enumerate(lines):
        for j, ch in enumerate(line):
            if ch in SHADES:
                draw.text((20 + j * char_w, 20 + i * line_h), ch,
                          font=font, fill=color_for(ch))
    frames.append(to_gif_frame(img))

frames[0].save(
    "asset/donut.gif", 
    save_all=True, 
    append_images=frames[1:],
    duration=33, 
    loop=0,
    transparency=255,
    disposal=2,
)
print(f"Saved donut.gif with {len(frames)} frames")