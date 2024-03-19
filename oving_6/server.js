const net = require('net');

const httpServer = net.createServer((connection) => {
  connection.on('data', () => {
    let content = `<!DOCTYPE html>
    <html>
    <head>
      <meta charset="UTF-8" />
      <title>Drawing App</title>
      <style>
        canvas {
          border: 1px solid black;
        }
      </style>
    </head>
    <body>
      <canvas id="drawingCanvas" width="800" height="600"></canvas>
      <script>
        let ws = new WebSocket('ws://localhost:3001');
        let canvas = document.getElementById('drawingCanvas');
        let ctx = canvas.getContext('2d');
        let drawing = false;
        
        canvas.addEventListener('mousedown', (event) => {
          drawing = true;
          ctx.beginPath();
          ctx.moveTo(event.offsetX, event.offsetY);
        });
        
        canvas.addEventListener('mousemove', (event) => {
          if (drawing) {
            ctx.lineTo(event.offsetX, event.offsetY);
            ctx.stroke();
            sendCanvasData();
          }
        });
        
        canvas.addEventListener('mouseup', () => {
          drawing = false;
        });
        
        canvas.addEventListener('mouseout', () => {
          drawing = false;
        });

        function sendCanvasData() {
          if (ws.readyState === WebSocket.OPEN) {
            let dataURL = canvas.toDataURL();
            ws.send(dataURL);
          } else {
            console.log('WebSocket is not connected.');
          }
        }

        ws.onmessage = function(event) {
          const message = event
          if (message.type === 'message') {
            updateCanvas(message.data);
          }
        };
        
        function updateCanvas(dataUrl) {
          const img = new Image();
          img.onload = function() {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ctx.drawImage(img, 0, 0);
          };
          img.src = dataUrl;
        }        

      </script>
    </body>
    </html>
    `;
    connection.write('HTTP/1.1 200 OK\r\nContent-Length: ' + content.length + '\r\n\r\n' + content);
  });
});
httpServer.listen(3000, () => {
  console.log('HTTP server listening on port 3000');
});




const crypto = require('crypto');

function generateAcceptValue(acceptKey) {
  return crypto.createHash('sha1')
               .update(acceptKey + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11', 'binary')
               .digest('base64');
}

function parseHeaders(request) {
  const headers = {};
  const lines = request.split('\r\n');
  lines.forEach(line => {
    const [key, value] = line.split(': ');
    if (key && value) {
      headers[key] = value;
    }
  });
  return headers;
}

function handshake(headers, socket) {
  const key = headers['Sec-WebSocket-Key'];
  const acceptKey = crypto.createHash('sha1').update(key + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11', 'binary').digest('base64');

  const responseHeaders = [
    'HTTP/1.1 101 Switching Protocols',
    'Upgrade: websocket',
    'Connection: Upgrade',
    `Sec-WebSocket-Accept: ${acceptKey}`,
    '\r\n'
  ];

  socket.write(responseHeaders.join('\r\n'));
}

function encodeMessage(msg) {
  const bytesRaw = Buffer.from(msg);
  let frameBuffer;
  if (bytesRaw.length <= 125) {
    frameBuffer = Buffer.alloc(2 + bytesRaw.length);
    frameBuffer[1] = bytesRaw.length;
  } else if (bytesRaw.length <= 65535) {
    frameBuffer = Buffer.alloc(4 + bytesRaw.length);
    frameBuffer[1] = 126;
    frameBuffer.writeUInt16BE(bytesRaw.length, 2);
  } else {
    frameBuffer = Buffer.alloc(10 + bytesRaw.length);
    frameBuffer[1] = 127;
    frameBuffer.writeBigUInt64BE(BigInt(bytesRaw.length), 2);
  }
  frameBuffer[0] = 129;
  bytesRaw.copy(frameBuffer, frameBuffer.length - bytesRaw.length);
  return frameBuffer;
}


function decodeMessage(buffer) {
  const secondByte = buffer[1];
  let length = secondByte & 127;
  let maskStart;
  let dataStart;

  if (length === 126) {
    length = buffer.readUInt16BE(2);
    maskStart = 4;
  } else if (length === 127) {
    length = Number(buffer.readBigUInt64BE(2));
    maskStart = 10;
  } else {
    maskStart = 2;
  }

  dataStart = maskStart + 4;
  let decoded = Buffer.alloc(length);

  for (let i = 0; i < length; i++) {
    const byte = buffer[dataStart + i] ^ buffer[maskStart + (i % 4)];
    decoded[i] = byte;
  }

  return decoded.toString();
}


const clients = [];

const wsServer = net.createServer(socket => {
  socket.on('data', data => {
    if (/^GET /m.test(data.toString())) {
      const headers = parseHeaders(data.toString());
      handshake(headers, socket);
      clients.push(socket);
    } else {
      const message = decodeMessage(data);
     
      clients.forEach(client => {
        if (client !== socket && client.writable) {
          client.write(encodeMessage(message));
        }
      });
    }
  });

  socket.on('end', () => {
    const index = clients.indexOf(socket);
    if (index !== -1) {
      clients.splice(index, 1);
    }
    console.log('Client disconnected');
  });

  socket.on('error', (err) => {
    console.error('Socket error', err);
  });
});

wsServer.on('error', (error) => {
  console.error('Server error:', error);
});

wsServer.listen(3001, () => {
  console.log('WebSocket server listening on port 3001');
});