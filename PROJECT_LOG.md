# Tank Arcade Game - Implementation Log

## Project Overview
A tank arcade game built with SFML (Simple and Fast Multimedia Library) featuring player-controlled tanks, enemy AI, and combat mechanics.

---

## Implemented Features

### 1. **Core Game Window**
- Window dimensions: 1280x720 pixels
- Title: "Tank Arcade"
- Frame rate managed with SFML clock system

### 2. **Tank System**
#### Player Tank
- Base `Tank` class with the following capabilities:
  - Texture-based rendering
  - WASD movement controls (up/down/left/right)
  - Rotation based on direction (0°, 90°, 180°, 270°)
  - Velocity-based movement system
  - HP system (default 100)
  - Fire mechanism with directional projectiles
  - Boundary clamping to keep player in map bounds

#### Enemy Tank (`EnemyTank`)
- Inherits from `Tank` class
- **AI State Machine:**
  - `PATROL`: Wanders around the map with random target points
  - `CHASE`: Moves toward player when in detection radius
  - `ATTACK`: Attacks player when in range
- **AI Behaviors:**
  - `checkDistance()`: Calculates distance to player
  - `defaultPatrol()`: Random wandering with target-based movement
  - `moveToPlayer()`: Pursues player within detection radius (stops at minimum range)
  - `shootAtPlayer()`: Calculates direction to fire at player
- **Current Spawns:**
  - Enemy 1: Top-left corner
  - Enemy 2: Bottom-right corner

### 3. **Bullet System**
- `Bullet` class with:
  - Position and direction tracking
  - Speed: 200 units/frame
  - Damage: 50 HP per hit
  - Ownership tracking (Player vs Enemy)
  - Shared texture loading for optimization
  - Collision bounds detection
  - Alive/dead state management

### 4. **Map System**
- Grid-based tile map (11 rows × 20 columns)
- **Tile Types:**
  - Sand tiles (walkable, tile value = 1)
  - Red tiles (walls, tile value = 0)
- **Border:** One-tile border of red tiles around the map edges
- **Textures:**
  - `sandbagBeige.png` - Sand/walkable area
  - `red.png` - Wall/boundary tiles
- Automatic texture scaling to fit 64x64 pixel tiles

### 5. **Menu System**
- `Menu` class with state management
- **Game States:**
  - `MENU`: Main menu
  - `SETTINGS`: Settings screen
  - `PLAY`: Active gameplay
  - `WIN`: Victory screen
- Basic structure in place (ready for expansion)

### 6. **Game Loop Mechanics**
- Delta time-based movement for smooth animations
- Shoot cooldown system (600ms between enemy shots)
- Bullet management with dynamic vector allocation (256 bullet reserve)
- Space bar shooting mechanic with state tracking
- Input handling for player movement and actions

### 7. **UI Elements**
- Title text: "Tank Arcade" (120pt, white, bold)
- Victory text: "You won!" (for win state)
- Instruction text: "[Press Enter to start]"
- Trophy sprite asset (`cup.png`) for win screen
- Text centering utility function
- Font system with Arial TTF loading

### 8. **Asset Management**
- Texture loading system for:
  - `tank.png` - Tank sprite
  - `bullet.png` - Bullet sprite
  - `sandbagBeige.png` - Sand tile
  - `red.png` - Wall tile
  - `cup.png` - Trophy sprite
- Font loading: `arial.ttf`
- Error handling for missing assets

---

## Project Structure
```
sfmlgame/
├── main.cpp          (Main game loop)
├── tank.h            (Player tank class)
├── enemy.h           (Enemy tank AI class)
├── bullet.h          (Projectile system)
├── map.h             (Tile-based map)
├── menu.h            (Game state management)
├── assets/
│   └── tanks/        (Tank-related assets)
├── enity/            (Entity-related assets folder)
└── app               (Compiled executable)
```

---

## Build Configuration
- **Compiler:** g++ with C++17 standard
- **SFML Libraries:** 3.0
- **Linked Libraries:**
  - sfml-graphics
  - sfml-window
  - sfml-system
  - sfml-audio
- **SFML Installation Path:** `/opt/sfml-3/`

---

## TODO / Next Steps
- [ ] Implement collision detection (bullet-tank, tank-wall)
- [ ] Complete enemy health system with death/respawn
- [ ] Implement scoring system
- [ ] Add sound effects and background music
- [ ] Expand menu with settings functionality
- [ ] Implement win/lose conditions
- [ ] Add visual feedback (explosions, hit effects)
- [ ] Fine-tune enemy AI difficulty levels
- [ ] Add powerups/items
- [ ] Optimize rendering and physics

---

**Last Updated:** May 23, 2026
