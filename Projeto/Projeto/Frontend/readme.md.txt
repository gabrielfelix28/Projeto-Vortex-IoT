# Frontend - Dashboard de Monitoramento

Dashboard web para visualização dos dados de temperatura, umidade e luminosidade em tempo real, consumindo a API do InfluxDB Cloud.

##  Tecnologias utilizadas

- HTML5
- CSS3
- JavaScript (vanilla)
- InfluxDB Cloud API (consulta)

##  Como executar

### Opção 1 - Abrir diretamente no navegador

1. Navegue até a pasta `Frontend/`
2. Abra o arquivo `index.html` em qualquer navegador moderno

### Opção 2 - Servidor local (recomendado)

```bash
# Se tiver Python instalado:
python -m http.server 8000

# Ou com Node.js:
npx serve .