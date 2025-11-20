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
          github_token: ${{ secrets.GITHUB_TOKEN }}

          repository_name: ${{ github.event.painel_monitoramento_hidrometros.name }}
          username: matos-ayrton
          
          show_icons: true
           number_of_days: 7 

      - name: Commit e Push
        run: |
          git config --global user.name 'github-actions[bot]'
          git config --global user.email 'github-actions[bot]@users.noreply.github.com'
          git add README.md
          git commit -m "Atualizar Gráfico de Atividade" || echo "Nenhuma alteração no README."
          git push
