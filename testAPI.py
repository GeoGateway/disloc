import requests


def fetch_url(url,payload):
    try:
        response = requests.get(url,params=payload)
        #print(realurl)
        return response.request.url, response.text, None
    except Exception as e:
        return response.request.url, None, e

def test_input(baseurl,inputfile):
    """test input file"""

    with open(inputfile,'r') as f:
        data = f.read()

    payload = {'input':data}
    
    result = fetch_url(baseurl,payload)
    print(result)




def main():
    serverurl = "http://localhost:5000/dislocservice/disloc"
    # good one
    disloc_input = "test/4fault.txt"
    test_input(serverurl,disloc_input)
    # bad one: timeout
    #disloc_input = "test/badinput.txt"
    #test_input(serverurl,disloc_input)


if __name__ == '__main__':
    main()
