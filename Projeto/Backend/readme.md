# Backend - InfluxDB Cloud

O backend deste projeto é composto pelo serviço gerenciado **InfluxDB Cloud**, que armazena e disponibiliza os dados de telemetria enviados pelo ESP32.

##  Tecnologia utilizada

- **InfluxDB Cloud** (versão 2.x)
- **API de escrita**: endpoint `/api/v2/write`
- **Line Protocol**: formato de envio dos dados

##  Estrutura dos dados

### Measurement
`sensor_ambiente`

### Tags
| Tag     | Valor               | Descrição                |
|---------|----------------------|--------------------------|
| device  | franzininho_lab01    | Identificador do ESP32   |
| local   | maquina1             | Local físico do sensor   |

### Fields
| Field         | Tipo   | Unidade      |
|---------------|--------|--------------|
| temperatura   | float  | °C           |
| umidade       | float  | %            |
| luminosidade  | float  | %            |

### Exemplo de linha enviada (Line Protocol)
