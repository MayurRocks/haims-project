
# Pi GPIO port which is connected to the button.
BUTTON_PIN = 25
# Down and up values for the button.  The code expects to detect a down to up
# transition as an activation of the button.  Therefore a normally open button
# should be False (low) when down and True (high) when up.
BUTTON_DOWN = False  # Low signal
BUTTON_UP   = True   # High signal

POSITIVE_THRESHOLD = 2000.0

# File to save and load face recognizer model.
TRAINING_FILE = 'training.xml'

# Directories which contain the positive and negative training image data.
POSITIVE_DIR = './training/positive'
NEGATIVE_DIR = './training/negative'

POSITIVE_LABEL = 1
NEGATIVE_LABEL = 2

FACE_WIDTH  = 92
FACE_HEIGHT = 112

HAAR_FACES         = 'haarcascade_frontalface_alt.xml'
HAAR_SCALE_FACTOR  = 1.3
HAAR_MIN_NEIGHBORS = 4
HAAR_MIN_SIZE      = (30, 30)

DEBUG_IMAGE = 'capture.pgm'

def get_camera():	
	import picam
	return picam.OpenCVCapture()
