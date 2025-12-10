# Swappable grug backend

## Running the program

1. Clone the repository to your machine.
2. Open either the `c` or `python` directory in VS Code, depending on which implementation you want to run.
3. Press **F5** to start the program.

   * If you want to switch backends, open the **Run and Debug** panel in VS Code and change the launch configuration from **Interpreter backend** to **Native backend**.

## Explanation

```mermaid
sequenceDiagram
    Game->>Bindings: grug.regenerate_modified_mods()
    Bindings->>Frontend: grug_dll.grug_regenerate_modified_mods()
    Frontend->>Backend: backend.compile_file()
    Backend->>Frontend: Return grug_file struct
    Frontend->>Bindings: grug_reloads_size++
    Bindings->>Game: 
```

```mermaid
sequenceDiagram
    Game->>Bindings: grug.get_mods()
    Bindings->>Frontend: return grug_dll.mods
    Frontend->>Bindings: TODO: Should this be casted to class Dir?
    Bindings->>Game: TODO: ?
```
