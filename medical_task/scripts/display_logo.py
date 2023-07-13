#!/usr/bin/env python2
from Tkinter import *

# creating tkinter window
root = Tk()
root.geometry("700x350")
root.configure(background="white")

# Adding widgets to the root window
Label(root, text = 'LSCM medical delivery robot',bg="white", font =(
'Verdana', 32)).pack(side = TOP, pady = 10)

# Creating a photoimage object to use image
photo = PhotoImage(file = "/home/lscm/deliverybot_ws/src/lscm_deliverybot/medical_task/include/lscm.png")
photoimage = photo.subsample(2,2)


def close_window():
    root.destroy()
    exit()
    
# set image on button
Label(root, image=photoimage, background="white").place(relx=.5, rely=.5,anchor= CENTER)

Button(root, text="Exit", font= "none 32",command=close_window, background="white").pack(side = BOTTOM)

root.attributes('-fullscreen',True)
mainloop()
