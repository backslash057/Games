from bs4 import BeautifulSoup
import requests

header = {"User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36"}
base = "https://www.sounds-resource.com/"

path = "https://www.sounds-resource.com/playstation_2/kingoffighters2003"
response = requests.get(path, headers=header)

print(f"Response: {len(response.text)}")
soup = BeautifulSoup(response.text, 'html.parser')

links = {}

for item in soup.find_all('td'):
	child = item.find("a")
	if child is not None and child.get_text().endswith("Voice"): links[base + child['href']] = child.get_text()

session = requests.Session()
for link in links:
	response = session.get(link, headers=header)

	if response.status_code != 200:
		print(link + ": failed")
		continue

	soup = BeautifulSoup(response.text, 'html.parser')

	download_btn = soup.find(class_="zip-download nomobile").parent
	filename = "sounds/" + links[link]
	download_path = base + download_btn['href']

	with session.get(download_path, stream=True, headers=header) as r:
		with open(filename, 'wb') as f:
			for chunk in r.iter_content(chunk_size=102400):
				f.write(chunk)
	print(filename, "completed")

