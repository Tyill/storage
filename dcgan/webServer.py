from flask import Flask, escape, request

webApp = Flask(__name__)

@webApp.route('/')
def load():
    name = request.args.get("name", "World")
    return f'Hello, {escape(name)}!'

@webApp.route('/api/getName')
def getName():
    return 'getName'