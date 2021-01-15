# GeoGateway Disloc Service

complile disloc.c:
	gcc disloc.c -o disloc -lm   
	
## Testing Environment:
* [Anaconda Python 3.8](https://docs.conda.io/en/latest/miniconda.html)  

## Command line mode:

**disloc_exec.py**    

   required parameters: 
<pre>
   -i,--input : disloc input file  
</pre>  
   optional:
<pre>
   * -o, --output : output file name prefix
   * -el, --elevation : elevation in degrees, default: 60
   * -az, --azimuth : azimuth in degrees, default: 0
   * -rf, --radarfrequency : radarfrequency: in GHz, default: 1.26
   * -wd, --workdir : working directory
</pre>
Output:   
<pre>
   * input.txt                  input file
   * output.csv			disloc output
   * output.csv.insar.kml	Synthetic Interferograms (kml)
   * output.csv.insar.kmz	Synthetic Interferograms (kmz)
   * output.csv.insar.png	Synthetic Interferograms (plot)   
   * summary.json               Job execution summary
 </pre>
Sample of summary.json     
```
disloc_exec -i 4fault.txt -az -5  
```
```json
{
    "status": "success",
    "error": "",
    "latlonbox": {
        "north": 33.51348223715415,
        "south": 32.6151708825009,
        "east": -114.84272682989001,
        "west": -116.97993517011
    },
    "parameters": {
        "elevation": 60,
        "azimuth": -5.0,
        "radarfrequency": 1.26
    },
    "output": [
        "input.txt",
        "output.csv.insar.kmz",
        "output.csv.insar.kml",
        "output.csv.insar.png",
        "output.csv",
        "4fault.txt",
        "summary.json"
    ]
}
```
