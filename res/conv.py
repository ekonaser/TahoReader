import cairosvg
from PIL import Image

# Convert SVG to PNG
cairosvg.svg2png(url='user.svg', write_to='temp.png', output_width=256, output_height=256)

# Convert PNG to ICO
img = Image.open('temp.png')
img.save('user.ico', format='ICO', sizes=[(256, 256)])
