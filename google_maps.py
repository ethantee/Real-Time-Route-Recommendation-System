import googlemaps
import pandas as pd

# Read mall names from the file
# Use a raw string for the file path
with open('C:\\Users\\ethan\\OneDrive - Asia Pacific University\\Desktop\\Real-Time Route Recommendation System\\malls.txt', 'r') as file:
    mall_names = [line.strip() for line in file if line.strip()]

 
# Initialize Google Maps client
gmaps = googlemaps.Client(key='API_KEY')
 
def get_distance(mall1, mall2):
    """Get the distance between two malls using Google Maps API."""
    try:
        distance_result = gmaps.distance_matrix(mall1, mall2, mode='driving')
        elements = distance_result['rows'][0]['elements'][0]
        if 'distance' in elements:
            distance = elements['distance']['value'] / 1000  # Convert to km
            return distance
        else:
            return None
    except Exception as e:
        print(f"Error calculating distance between {mall1} and {mall2}: {e}")
        return None
 
# Calculate all distances
results = []
for mall1 in mall_names:
    for mall2 in mall_names:
        distance = get_distance(mall1, mall2)
        if distance is not None:
            results.append({'Mall 1': mall1, 'Mall 2': mall2, 'Distance (km)': distance})
 
# Save results to CSV
results_df = pd.DataFrame(results)
results_df.to_csv('mall_distances.csv', index=False)