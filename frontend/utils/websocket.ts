export class WebSocketClient {
  private ws: WebSocket

  constructor() {
    this.ws = new WebSocket('ws://localhost:8080')
    this.setupListeners()
  }

  private setupListeners() {
    this.ws.onmessage = (event) => {
      const data = JSON.parse(event.data)
      console.log('收到消息:', data) // 处理消息
    }
  }
}
