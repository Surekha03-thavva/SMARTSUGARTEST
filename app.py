from flask import Flask, request
import pickle
with open('model.pkl','rb') as f:
    ai=pickle.load(f) # type: ignore
app=Flask(__name__)
@app.route('/')
def default():
    return 'AI Server Started'
@app.route('/predict', methods=['GET'])
def predict():
    G=float(request.args.get('glucose_level'))
    result=ai.predict([[G]])[0]
    return result
if __name__ =="__main__":
    app.run(host='0.0.0.0',port=2000)