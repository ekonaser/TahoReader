import cairosvg
from PIL import Image

# Convert SVG to PNG
cairosvg.svg2png(url='administration.svg', write_to='temp.png', output_width=64, output_height=64)

# Convert PNG to ICO
img = Image.open('temp.png')
img.save('administration.ico', format='ICO', sizes=[(64, 64)])
