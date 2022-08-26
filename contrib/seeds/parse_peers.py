import requests

MIN_HEIGHT = 550000
PORT = 9998

def parse():
    r = requests.get("https://equitypay.online/data/peers")

    for peer in r.json()["result"]:
        if peer["height"] >= MIN_HEIGHT:
            address = peer["address"]
            print(f"{address}:{PORT}")


if __name__ == "__main__":
    parse()
