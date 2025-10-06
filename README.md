┌─────────────────┐
│     INÍCIO      │
└─────────┬───────┘
          │
┌─────────▼───────┐
│  Inicialização  │
│  - AudioManager │
│  - Highscore    │
│  - Background   │
└─────────┬───────┘
          │
┌─────────▼─────────────┐
│   TELA DE INÍCIO     │
│  - Mostra botão      │
│  - Toca som início   │
└─────────┬─────────────┘
          │
┌─────────▼─────────────┐
│  Usuário clica JOGAR? │◄────────────────┐
└─────────┬─────────────┘                 │
          │ SIM                           │
          │                               │
┌─────────▼─────────────┐                 │
│  INICIA JOGO PRINCIPAL│                 │
│  - Toca música fundo  │                 │
│  - Abre câmera        │                 │
└─────────┬─────────────┘                 │
          │                               │
┌─────────▼─────────────┐                 │
│     LOOP PRINCIPAL    │                 │
│  ┌───────────────────┐│                 │
│  │ Captura frame     ││                 │
│  │ da câmera         ││                 │
│  └─────────┬─────────┘│                 │
│            │          │                 │
│  ┌─────────▼─────────┐│                 │
│  │ Detecção facial   ││                 │
│  │ com Haar Cascade  ││                 │
│  └─────────┬─────────┘│                 │
│            │          │                 │
│  ┌─────────▼─────────┐│                 │
│  │ Gera objetos      ││                 │
│  │ caindo            ││                 │
│  └─────────┬─────────┘│                 │
│            │          │                 │
│  ┌─────────▼─────────┐│                 │
│  │ Verifica colisões ││                 │
│  │ objeto × rosto    ││                 │
│  └─────────┬─────────┘│                 │
│            │          │                 │
│  ┌─────────▼─────────┐│                 │
│  │ Atualiza estado   ││                 │
│  │ - Score ↑ se coletou │               │
│  │ - Vida ↓ se perdeu │                 │
│  └─────────┬─────────┘│                 │
│            │          │                 │
│  ┌─────────▼─────────┐│                 │
│  │ Desenha interface ││                 │
│  │ - Score           ││                 │
│  │ - Vidas (corações)││                 │
│  └─────────┬─────────┘│                 │
└─────────┬─────────────┘                 │
          │                               │
┌─────────▼─────────────┐                 │
│   Vidas == 0?        │                 │
└─────────┬─────────────┘                 │
          │ SIM                           │
          │                               │
┌─────────▼─────────────┐                 │
│     GAME OVER         │                 │
│  - Para música        │                 │
│  - Salva recorde      │                 │
└─────────┬─────────────┘                 │
          │                               │
┌─────────▼─────────────┐                 │
│   TELA GAME OVER      │                 │
│  - Mostra score       │                 │
│  - Toca som game over │                 │
└─────────┬─────────────┘                 │
          │                               │
┌─────────▼─────────────┐                 │
│  Jogar novamente?     ├─────────────────┘
└─────────┬─────────────┘
          │ NÃO
          │
┌─────────▼─────────────┐
│         FIM           │
└───────────────────────┘


Eventos de Áudio:
├── Durante jogo: playBackgroundMusic() (loop)
├── Coletar objeto: playCollectSound()
├── Perder vida: playHeartLostSound()
├── Game Over: playGameOverSound() + stopBackgroundMusic()
└── Reiniciar: stopBackgroundMusic() + playBackgroundMusic()
