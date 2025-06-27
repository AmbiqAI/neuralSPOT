import os
import keras
import tensorflow as tf
os.environ["KERAS_BACKEND"] = "tensorflow"
# import neuralspot_edge as nse
import numpy as np

num_test_samples = 1000
feat_shape = (224, 224, 3)
batch_size = 1
num_classes = 1000

quantization = "INT8"
io_type = "int8"
export_mode = "CONCRETE"

tfl_model_path = "./mobilenet_v3_small_quant.tflite"
tflm_model_path = "./mobilenet_v3_small_quant.h"
tflm_var_name = "mobilnet_v3_small_quant_flatbuffer"

# Load model
mn_model = keras.applications.MobileNetV3Small(
    input_shape=None,
    alpha=1.0,
    minimalistic=False,
    include_top=True,
    weights="imagenet",
    input_tensor=None,
    classes=1000,
    pooling=None,
    dropout_rate=0.2,
    classifier_activation="softmax",
    include_preprocessing=True,
    # name="MobileNetV3Small",
)

converter = tf.lite.TFLiteConverter.from_keras_model(mn_model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
# converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
# converter.inference_input_type = tf.int8
# converter.inference_output_type = tf.int8
# tflite_model_quantized = converter.convert()
tflite_model = converter.convert()
with open('converted_model.tflite', 'wb') as f:     
  f.write(tflite_model)


# Fix input and batch size
inputs = keras.Input(shape=feat_shape, batch_size=batch_size, dtype="float32")
outputs = model(inputs)

# Get model summary
# flops = nse.metrics.flops.get_flops(model, batch_size=batch_size, fpath=os.devnull)
# print(f"FLOPS: {flops/1e6:0.2} MFLOPS")

# Create random input samples
# test_x = np.random.rand(num_test_samples, *feat_shape).astype(np.float32)

# # Convert model
# converter = nse.converters.tflite.TfLiteKerasConverter(model=model)
# converter = tflite.TfLiteKerasConverter(model=model)

# tflite_content = converter.convert(
#     test_x=test_x,
#     quantization=quantization,
#     io_type=io_type,
#     mode=export_mode,
#     strict=True,
# )

# # Export model
# converter.export(tfl_model_path)
# converter.export_header(tflm_model_path, name=tflm_var_name)