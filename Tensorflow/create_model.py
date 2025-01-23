import os
import tensorflow as tf

# Disable CUDA (ignore GPU and use CPU only)
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"
# Suppress TensorFlow logs
os.environ["TF_CPP_MIN_LOG_LEVEL"] = "2"

# Define the model
input_1 = tf.keras.Input(shape=(5,))
output_1 = tf.keras.layers.Dense(5, activation=tf.nn.relu)(input_1)
output_1 = tf.keras.layers.Dense(1, activation=tf.nn.sigmoid)(output_1)
model = tf.keras.Model(inputs=input_1, outputs=output_1)

# Compile the model
model.compile(
    optimizer=tf.keras.optimizers.Adam(),
    loss=tf.keras.losses.BinaryCrossentropy(),
    metrics=[tf.keras.metrics.BinaryAccuracy()]
)

# Save the model
model.export("model")

print("Model saved successfully.")