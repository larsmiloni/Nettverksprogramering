<template>
  <div class="main">
    <h1>Code Compiler</h1>
    <textarea v-model="code" class="code-input" placeholder="Write your code here..."></textarea>
    <button @click="runCode" class="compile-button">Compile Code</button>
    <div v-if="output" class="output-container">
      <h2>Compilation Result:</h2>
      <pre class="output">{{ output }}</pre>
    </div>
  </div>
</template>

<style scoped>
.main {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 20px;
  font-family: Arial, sans-serif;
}

.code-input {
  width: 600px;
  height: 250px;
  margin-bottom: 20px;
  font-size: 16px;
  padding: 10px;
  border: 2px solid #ccc;
  border-radius: 4px;
}

.compile-button {
  width: 200px;
  height: 50px;
  font-size: 18px;
  color: white;
  background-color: #007bff;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  transition: background-color 0.2s ease;
}

.compile-button:hover {
  background-color: #0056b3;
}

.output-container {
  margin-top: 20px;
  text-align: left;
  width: 600px;
}

.output {
  white-space: pre-wrap;
  word-wrap: break-word;
  background-color: #f8f9fa;
  border: 1px solid #ddd;
  border-radius: 4px;
  padding: 10px;
  font-size: 14px;
  color: #333;
}
</style>

<script setup>
import { ref } from 'vue';

const code = ref('');
const output = ref(null);

const runCode = async () => {
  const apiUrl = 'http://localhost:5000/run-code';
  try {
    const response = await fetch(apiUrl, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ code: code.value })
    });
    const data = await response.json();
    console.log("Dette er output:", data);
    output.value = data.output; // Oppdaterer output-referansen med responsdata
  } catch (error) {
    console.error('Error:', error);
  }
};
</script>