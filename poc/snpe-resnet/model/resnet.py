import torch
import torchvision.models as models

# Load a pre-trained ResNet-18 model
resnet18_model = models.resnet18(pretrained=True)

# Prepare input data with the expected shape [batch_size, channels, height, width]
input_shape = [1, 3, 224, 224]  # Example input shape
input_data = torch.randn(input_shape)

# Trace the model with input data
script_model = torch.jit.trace(resnet18_model, input_data)

# Save the TorchScript model to a file
script_model.save("resnet18.pt")

print("TorchScript model (ResNet-18) saved as resnet18.pt")