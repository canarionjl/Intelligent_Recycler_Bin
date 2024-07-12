from flask import Flask, make_response, request
from PIL import Image

import base64
import io
import google.generativeai as genai

API_KEY = "AIzaSyAEEZ3Ka8ZYpO3TtQQuhxEqfdgxmy_b6Ig"

genai.configure(api_key=API_KEY)
model = genai.GenerativeModel("gemini-pro-vision")

bin_list = ["Marron", "Azul", "Verde", "Amarillo"]

def get_bin_by_model(img):

    response = model.generate_content(
        [
           "¿En qué contenedor de basura se debe tirar este residuo? \
            Respóndeme escogiendo una de esta opciones y escribiendo únicamente la palabra entre comillas en la respuesta: \
             \"Azul\" si el residuo lo clasificas como cartón o papel \
             \"Amarillo\" si el residuo lo clasificas como plástico o latas \
             \"Marron\" (sin tilde, para evitar errores de codificación) si el residuo lo clasificas como orgánico \
              \"Verde\" si el residuo lo clasificas como vidrio \
            En cualquier caso, asegúrate siempre de que tu respuesta sea solamente una de las cuatro palabras que \
             te comento entre comillas arriba. Si no tienes muy muy clara la respuesta sobre un residuo, la opción por defecto es \"Marron\" \
          "
         , img],
         stream=True
    )
    response.resolve()
    bin = evaluate_response(response.text)
    return bin

def evaluate_response (response):
   if response in bin_list:
      return response
   else:
      return "Amarillo"


# ------------------------------ REQUESTS ------------------------- #

import firebase_admin
from firebase_admin import credentials, storage
import uuid

cred = credentials.Certificate(
   "C:/Users/canar/Desktop/ASIOT/server/google-services.json"
)

firebase_admin.initialize_app(
   cred, 
   {"storageBucket":"iotproject-48058.appspot.com"}
)


def upload_image():
  uuid_ = uuid.uuid4()
  bucket = storage.bucket()
  blob = bucket.blob(f"plastico/{uuid_}.jpg")
  blob.upload_from_filename("imagen_recibida.jpg", content_type="image/jpeg")


# ------------------------------ SERVER CONFIG ------------------------- #


app = Flask(__name__)


@app.route("/")
def home():
  return make_response("Route not available", 503)

@app.route("/requestBin", methods=['POST'])
def requestBin():

  datos = request.get_json()
  imagen_processed = True

  if datos is None: 
     imagen_processed = False
  
  try:
    imagen_base64 = datos['imagen']
    imagen_bytes = base64.b64decode(imagen_base64)
    imagen = Image.open(io.BytesIO(imagen_bytes))
    imagen.save('imagen_recibida.jpg')
    upload_image()

  except Exception as e:
     imagen_processed = False

  if imagen_processed:
      try:
        bin = get_bin_by_model(img=imagen)
      except Exception as e:
        bin = "Error"
  else:
     bin = "Error"

  print("\nEl contenedor es: ",bin,"\n")
  return bin



if __name__  == "__main__":
  app.run(host='0.0.0.0', port=5000, debug=True)
 