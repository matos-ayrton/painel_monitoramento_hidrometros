name: Atualizar Gráfico de Atividade do Projeto

on:
  schedule:
    # Roda a cada 12 horas. Você pode ajustar a frequência.
    # CRON: min hour day_of_month month day_of_week
    - cron: '0 */12 * * *'
  workflow_dispatch:

jobs:
  update-activity:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: Ashutosh00710/github-readme-activity-graph@v2.0.0
        with:
          # Seu token de acesso pessoal (necessário para ler dados, especialmente se o repositório for privado)
          # Use o token de ambiente GITHUB_TOKEN padrão do GitHub Actions, que é suficiente para repos públicos.
          github_token: ${{ secrets.GITHUB_TOKEN }}
          
          # O NOME DO REPOSITÓRIO ONDE ESTE ACTION ESTÁ RODANDO
          # Isso garante que ele rastreie APENAS a atividade deste projeto.
          repository_name: ${{ github.event.painel_monitoramento_hidrometros.name }}
          
          # O username que será rastreado
          # Use seu nome de usuário aqui para rastrear apenas sua atividade neste projeto
          username: matos_ayrton
          
          # Outras configurações opcionais
          show_icons: true
          # Para ver a atividade dos últimos 7 dias (opcional, por padrão são 31)
           number_of_days: 7 

      - name: Commit e Push
        run: |
          git config --global user.name 'github-actions[bot]'
          git config --global user.email 'github-actions[bot]@users.noreply.github.com'
          git add README.md
          git commit -m "Atualizar Gráfico de Atividade" || echo "Nenhuma alteração no README."
          git push
