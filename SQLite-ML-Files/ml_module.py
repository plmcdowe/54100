# ml_module.py
import joblib
from transformers import pipeline
import numpy as np
from sklearn.ensemble import IsolationForest
import pickle
from collections import Counter
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'  # Suppress TensorFlow logs
os.environ['CUDA_VISIBLE_DEVICES'] = '' #cpu only
import sqlite3

embedding_pipeline = pipeline("feature-extraction", model="distilbert-base-uncased")

def train_categorical_outlier_model(data):
    # Calculate frequency-based outlier detection model
    #frequency_count = Counter(data)
    #total_count = len(data)
    #frequency_score = {key: count / total_count for key, count in frequency_count.items()}
    #return pickle.dumps(frequency_score)
    
    #using feature extraction instead
    embeddings = np.array([embedding_pipeline(item)[0][0] for item in data])
    # Train Isolation Forest on embeddings
    model = IsolationForest(n_estimators=100, contamination=0.05, random_state=42)
    model.fit(embeddings)
    
    #scores = model.decision_function(data_array)
    # Serialize the model
    serialized_model = pickle.dumps(model)  # Replace with joblib if desired: joblib.dumps(model)
    print(f"Serialized model size: {len(serialized_model)} bytes")  # Debugging
    # Debug: Print the mean and variance of decision scores
    assert isinstance(serialized_model, bytes), "Serialized model should be of type bytes."
    return serialized_model

def train_and_serialize_model(data):
    # Ensure data is a 1D array and handle NaNs or infinite values
    random_state = np.random.RandomState(42)
    data = np.array(data)
    data = [round(x,1) for x in data]
    print(data)
    if np.isnan(data).any() or np.isinf(data).any():
        raise ValueError("Data contains NaN or infinite values.")
    
    data_array = np.array(data).reshape(-1, 1)
    model=IsolationForest(contamination=0.004,random_state=random_state)
    #model = IsolationForest(contamination=0.1, n_estimators=50)
    model.fit(data_array)
    print(data_array)
    # Serialize the model
    serialized_model = pickle.dumps(model)  # Replace with joblib if desired: joblib.dumps(model)
    print(f"Serialized model size: {len(serialized_model)} bytes")  # Debugging
    # Debug: Print the mean and variance of decision scores
    scores = model.decision_function(data_array)
    print(f"Decision score range: {scores.min()} to {scores.max()}")
    assert isinstance(serialized_model, bytes), "Serialized model should be of type bytes."
    return serialized_model

def is_outlier_with_model_data(value, model_data, data_type):

    # Deserialize model data to retrieve the model and data type
    try:
        #print(f"Loading Model:{value[0]}")
        model = pickle.loads(model_data)  # Deserialize model
    except (pickle.UnpicklingError, EOFError) as e:
        raise ValueError("Failed to deserialize model data.") from e

    if data_type == "numeric":
        # Numeric outlier detection using IsolationForest
        try:
            # Convert value to float for numeric models
            numeric_value = round(value,1)
        except ValueError:
            print(f"ValueError: Cannot convert '{value}' to float for numeric model.")
            return 0  # Return 0 (not an outlier) if conversion fails
        #print(numeric_value)
        #score = model.predict([[numeric_value]])[0]
        score = model.decision_function([[numeric_value]])[0]
        #print(score)
        return round(float(score),2)
        #min_score, max_score = -1.0, 1.0 
        #normalized_score = 2 * ((score - min_score) / (max_score - min_score)) - 1
        #return round(max(-1, min(1, normalized_score)),2)  # Return the score in the range [-1, 1]

    elif data_type == "categorical":
        # Categorical outlier detection using frequency-based model
        #rarity_threshold = 0.01  # Define rarity threshold, e.g., 1%
        #frequency_score = model.get(value, 0)  # Get frequency score for the category
        #is_outlier = frequency_score < rarity_threshold
        #return float(is_outlier)  # Return 1 if outlier, 0 otherwise

        #returning the embedded feature extraction
        embedding = embedding_pipeline(value)[0][0]
        # Get outlier score
        score = model.decision_function([embedding])[0]
        #return float(score)
        # Normalize score between -1 and 1
        return float(score)
        #min_score, max_score = -1.0, 1.0  # Adjust based on your model
        #normalized_score = 2 * ((score - min_score) / (max_score - min_score)) - 1
        #return round(max(-1, min(1, normalized_score)), 1)
    else:
        print(f"Unsupported data type: {data_type}")
        return 0
    

#Load sentiment analysis model from Hugging Face
sentiment_analyzer = pipeline("sentiment-analysis",device=-1, model="distilbert-base-uncased-finetuned-sst-2-english")
def analyze_sentiment(text):
    # Test sentiment analysis on text
    result = sentiment_analyzer(text)[0]
    label = result['label']
    score = result['score']
    return label, score