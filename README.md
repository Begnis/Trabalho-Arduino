# Trabalho-Arduino
/*
 * ============================================================
 *  ESP32 - Coordenação de Laboratórios
 * ============================================================
 *  Missão:
 *   - Conecta ao Wi-Fi Wokwi-GUEST (sem senha)
 *   - Conecta ao broker MQTT da turma
 *   - Assina Lab1/# até Lab5/#
 *   - Processa tópicos no formato LabX/AlunoYY
 *   - Armazena o último valor (0 ou 1) de cada aluno (01-10)
 *     para cada laboratório (1-5) em uma matriz 2D
 *   - Calcula a média de atenção por laboratório
 *   - Controla 5 LEDs (um por lab):
 *       média < 0.5  → LED aceso  (alerta)
 *       média >= 0.5 → LED apagado (ok)
 *
 *  Mapeamento de LEDs:
 *   Lab1 → GPIO 2
 *   Lab2 → GPIO 4
 *   Lab3 → GPIO 5
 *   Lab4 → GPIO 18
 *   Lab5 → GPIO 19
 * ============================================================
 */
