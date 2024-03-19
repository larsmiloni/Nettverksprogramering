import subprocess
from flask import Flask, request, jsonify
from flask_cors import CORS

app = Flask(__name__)
CORS(app)

@app.route('/run-code', methods=['POST'])
def run_code():
    code = request.json.get('code')
    if not code:
        return jsonify({'error': 'No code provided'}), 400

    with open('user_script.py', 'w') as file:
        file.write(code)

    try:
        print("Starting Docker build...")
        subprocess.run(["docker", "build", "-t", "code-runner", "."], check=True)
        print("Build done!")
        print("Starting Docker run...")
        result = subprocess.run(["docker", "run", "--rm", "code-runner"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=False)
        if result.returncode == 0:
            print("Run complete.")
            output = result.stdout
        else:
            print("Run encountered an error.")
            output = result.stderr
            error_lines = result.stderr.split('\n')
            formatted_error = "\n".join(error_lines[2:])
            return jsonify({'output': formatted_error}), 400
    except subprocess.CalledProcessError as e:
        return jsonify({'output': output}), 500

    return jsonify({'output': output})

if __name__ == '__main__':
    app.run(debug=False)
