# Sistema de Preenchimento de Polígonos - ET/AET

## Compilação no Windows

### Método 1: Script Automático (Recomendado)
1. Abra o Prompt de Comando ou PowerShell
2. Navegue até a pasta `t1CG`
3. Execute: `compilar.bat`
4. O executável `main.exe` será gerado automaticamente

### Método 2: Compilação Manual
1. Abra o Prompt de Comando ou PowerShell
2. Navegue até a pasta `t1CG`
3. Execute o comando:
   ```
   g++ -o main.exe main.cpp auxcode/*.h -lgdi32 -luser32 -lkernel32
   ```

### Requisitos
- Compilador GCC/G++ (TDM-GCC, MinGW, ou similar)
- Windows 10/11

### Execução
- Execute o arquivo `main.exe` gerado
- Ou digite `main.exe` no terminal

### Funcionalidades
- Desenho de polígonos com clique do mouse
- Preenchimento usando algoritmo ET/AET
- Paleta de 16 cores
- Interface com botões interativos
- Salvar e carregar polígonos

### Controles
- **Clique Esquerdo**: Adicionar vértice
- **Clique Direito**: Fechar polígono
- **F**: Fechar polígono
- **C**: Limpar polígono
- **P**: Preencher polígono
- **V**: Alternar visibilidade dos vértices
- **+/-**: Alterar espessura da linha
- **1-6**: Alterar cor do preenchimento
- **S**: Salvar polígono
