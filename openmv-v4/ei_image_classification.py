# Edge Impulse - OpenMV Image Classification Example

import sensor, image, time, os, tf, uos, gc,lcd
from pyb import UART

sensor.reset()                         # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565)    # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QQVGA2)      # Set frame size to QVGA (320x240) QQVGA2(128x160)
sensor.set_windowing((240, 240))       # Set 240x240 window.
sensor.skip_frames(time=2000)          # Let the camera adjust.
#串号号+波特率


net = None
labels = None
count1=0
count2=0
count3=0
count4=0

try:
    # load the model, alloc the model file on the heap if we have at least 64K free after loading
    net = tf.load("trained.tflite", load_to_fb=uos.stat('trained.tflite')[6] > (gc.mem_free() - (64*1024)))
except Exception as e:
    print(e)
    raise Exception('Failed to load "trained.tflite", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')

try:
    labels = [line.rstrip('\n') for line in open("labels.txt")]
except Exception as e:
    raise Exception('Failed to load "labels.txt", did you copy the .tflite and labels.txt file onto the mass-storage device? (' + str(e) + ')')


clock = time.clock()
#uart = UART(1,9600)
lcd.init() # Initialize the lcd screen.


while(True):

    clock.tick()
    img = sensor.snapshot()
    # default settings just do one detection... change them to search the image...
    for obj in net.classify(img, min_scale=1.0, scale_mul=0.8, x_overlap=0.5, y_overlap=0.5):
        #print("**********\nPredictions at [x=%d,y=%d,w=%d,h=%d]" % obj.rect())
        img.draw_rectangle(obj.rect())
        # This combines the labels and confidence values into a list of tuples
        predictions_list = list(zip(labels, obj.output()))
        #uart.write("Hello world\r")        #发送数据
        for i in range(len(predictions_list)):
            #print("%s = %f" % (predictions_list[i][0], predictions_list[i][1]))
            #print(i)
            if i==0 and predictions_list[i][1]>0.8:
                img.draw_string(10, 20, "Leaf_mites", color = (255, 0,0),scale = 1,mono_space = False)
                img.draw_rectangle(10, 40, 80, 50, color = (255, 0, 0),thickness = 2, fill = False)
                count1+=1
                if count1==10:
                    #uart.write("1")        #发送数据
                    count1=0
                    print("*********Leaf_mites************")

            elif i==1 and predictions_list[i][1]>0.6:
                img.draw_string(10, 20, "Red_spiders", color = (255, 0,0),scale = 1,mono_space = False)
                img.draw_rectangle(10, 40, 70, 60, color = (255, 0, 0),thickness = 2, fill = False)
                count2+=1
                if count2==3:
                    #uart.write("2")        #发送数据
                    count2=0
                    print("**********Red_spiders***********")

            elif i==2 and predictions_list[i][1]>0.95:
                img.draw_string(10, 20, "Aphids", color = (255, 0,0),scale = 1,mono_space = False)
                img.draw_rectangle(10, 40, 50, 50, color = (255, 0, 0),thickness = 2, fill = False)
                count3+=1
                if count3==15:
                    #uart.write("3")        #发送数据
                    count3=0
                    print("************Aphids**************")

            elif i==3 and predictions_list[i][1]>0.7:
                img.draw_string(10, 20, "Whitefly", color = (255, 0,0),scale = 1,mono_space = False)
                img.draw_rectangle(10, 40, 50, 80, color = (255, 0, 0),thickness = 2, fill = False)
                count4+=1
                if count4==20:
                    #uart.write("4")        #发送数据
                    count4=0
                    print("************Whitefly**************")
        lcd.display(img)
        #print(clock.fps(), "fps")
