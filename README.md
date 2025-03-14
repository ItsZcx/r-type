# R-Type

**A multiplayer shoot 'em up game inspired by the classic R-Type series.**

## Table of Contents

- [About the Project](#about-the-project)
- [Features](#features)
- [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installation](#installation)
- [Documentation](#documentation)
- [Technologies Used](#technologies-used)
- [License](#license)
- [Contact](#contact)
    - [Team Members](#team-members)
- [Acknowledgments](#acknowledgments)

## About the Project

The R-Type Project is a modern take on the classic arcade game R-Type, featuring multiplayer capabilities and enhanced graphics. Players can team up to fight against waves of enemies and challenging bosses in a side-scrolling shooter environment.

The application is divided into two parts:
- Game server to handle the game logic and multiplayer interactions.
- A desktop client to provide a user interface for playing the game.

## Features

* **Multiplayer Support:** Play with friends in cooperative mode.
   
* **Nostalgic Graphics:** Pixel art visuals to retain the classic feel.
   
* **Boss Battles:** A challenging boss with two attack phases.
   
## Getting Started

Follow these instructions to get a copy of the project on your local machine.

### Prerequisites

- **CMake:** For managing the build process.
- **Make:** For building the project.
- **Conan:** For dependency management.
- **C++:** The programming language used for development.

### Installation

1. Clone the repository:
     ```bash
     git clone https://github.com/ItsZcx/r-type.git
     ```
2. Navigate to the project directory:
     ```bash
     cd r-type
     ```
3. Set up environment variables:
     ```bash
     cp .env.example .env
     ```

### Usage

To run the project, you can use the provided Makefile which includes commands to build and run both the server and the client.

1. **Build the project:**
    ```bash
    make build
    ```

2. **Run the server:**
    ```bash
        ./server [PORT]
    ```

3. **Run the client:**
    ```bash
        ./client
    ```

When you run the client, it will prompt you to enter the server IP. If you are running both the server and client on the same machine, provide the IP in the format `127.0.0.1:PORT`. Otherwise, provide the appropriate server IP and port.

## Documentation

For more detailed information, refer to the following documents in the `./docs` folder:
- [DEVELOPER.md](./docs/DEVELOPER.md)
- [ARCHITECTURE.md](./docs/ARCHITECTURE.md)
- [PROTOCOL.md](./docs/PROTOCOL.md)

## Technologies Used

**General:**
- **C++:** The programming language used for development.
- **Make:** For building the project.
- **CMake:** For managing the build process.
- **Conan:** For dependency management.
- **clang-format:** For code formatting.

**Networking:**
- **Asio:** For asynchronous networking.

**Graphics:**
- **SFML:** Simple and Fast Multimedia Library for graphics and audio. (You might have to install this manually)

## License

This project is licensed under the **MIT** License - see the [LICENSE](./LICENSE) file for details.

## Contact

### Team Members

- **Joan Pau Mérida Ruiz**: *Backend Developer, Infra Manager* ([GitHub](https://github.com/itszcx), [LinkedIn](https://www.linkedin.com/in/joan-pau-merida-ruiz), [Email](mailto:joanpaumeridaruiz@gmail.com))
- **Alex Arteaga Contijoch**: *Backend Developer* ([GitHub](https://github.com/alex-alra-arteaga), [LinkedIn](https://www.linkedin.com/in/alex-arteaga-c/), [Email](mailto:alex.arteaga-contijoch@epitech.eu))
- **David Salvatella Gelpi**: *Fullstack Developer* ([GitHub](https://github.com/xRozzo), [LinkedIn](hthttps://www.linkedin.com/in/david-salvatella/), [Email](mailto:david.salvatella-gelpi@epitech.eu))
- **Matheus López**: *Frontend Developer* ([GitHub](https://github.com/MatheusFreixo), [LinkedIn](https://www.linkedin.com/in/matheusfreixo/), [Email](mailto:matheus.lopez@epitech.eu))
- **Mario Alessandro**: *Mobile Developer* ([GitHub](https://github.com/MarioHM2004), [LinkedIn](https://www.linkedin.com/in/mario-herranz-b47512261/), [Email](mailto:mario-alessandro.herranz-machado@epitech.eu))

## Acknowledgments

- This project was inspired by the classic R-Type game series.
