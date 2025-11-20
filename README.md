## 📊 Atividade do Repositório

![Repo Activity](./repo-activity.svg)

name: Generate Repo Activity

on:
  schedule:
    - cron: "0 */6 * * *"  # a cada 6 horas
  workflow_dispatch:
  push:

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - uses: actions/checkout@v3

      - name: Install jq
        run: sudo apt-get install jq -y

      - name: Fetch repo activity data
        env:
          REPO: ${{ github.repository }}
          TOKEN: ${{ secrets.GITHUB_TOKEN }}
        run: |
          mkdir -p activity
          
          # Últimos 30 dias
          SINCE=$(date -u -d "30 days ago" +%Y-%m-%dT%H:%M:%SZ)

          commits=$(curl -s \
            -H "Authorization: token $TOKEN" \
            "https://api.github.com/repos/$REPO/commits?since=$SINCE" | jq length)

          issues=$(curl -s \
            -H "Authorization: token $TOKEN" \
            "https://api.github.com/repos/$REPO/issues?since=$SINCE&state=all" | jq length)

          prs=$(curl -s \
            -H "Authorization: token $TOKEN" \
            "https://api.github.com/repos/$REPO/pulls?state=all" | jq length)

          LOC=$((commits * 20)) # aproximação simbólica só pra visual

          echo "COMMITS=$commits" >> activity/data.txt
          echo "ISSUES=$issues" >> activity/data.txt
          echo "PRS=$prs" >> activity/data.txt
          echo "LOC=$LOC" >> activity/data.txt

      - name: Build SVG
        run: |
          source activity/data.txt

          echo "<svg xmlns='http://www.w3.org/2000/svg' width='500' height='200'>
            <rect width='500' height='200' fill='#fff'/>
            
            <text x='10' y='30' font-size='20'>Atividade dos últimos 30 dias</text>

            <text x='10' y='70' font-size='14'>Commits: $COMMITS</text>
            <rect x='150' y='60' width='$((COMMITS * 4))' height='12' fill='#4caf50'/>

            <text x='10' y='110' font-size='14'>Issues: $ISSUES</text>
            <rect x='150' y='100' width='$((ISSUES * 4))' height='12' fill='#2196f3'/>

            <text x='10' y='150' font-size='14'>PRs: $PRS</text>
            <rect x='150' y='140' width='$((PRS * 4))' height='12' fill='#9c27b0'/>

          </svg>" > repo-activity.svg

      - name: Commit SVG
        run: |
          git config --global user.name "github-actions"
          git config --global user.email "github-actions@github.com"
          git add repo-activity.svg
          git commit -m "Update repo activity" || echo "No changes"
          git push
