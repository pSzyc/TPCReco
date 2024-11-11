# Imports
import tensorflow as tf

input_1 = tf.keras.Input(shape=(5,))

output_1 = tf.keras.layers.Dense(5, activation=tf.nn.relu)(input_1)
output_1 = tf.keras.layers.Dense(1, activation=tf.nn.sigmoid)(output_1)
model = tf.keras.Model(inputs=input_1, outputs=output_1)

model.compile()

# Export the model to a SavedModel
model.save('model', save_format='tf')