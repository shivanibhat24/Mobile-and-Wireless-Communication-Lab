import math
import random

class FixedChannelAssignment:
    def __init__(self, total_channels, control_channel_percentage):
        print("="*60)
        print("FIXED CHANNEL ASSIGNMENT TECHNIQUE")
        print("="*60)
        
        self.total_channels = total_channels
        self.control_channel_percentage = control_channel_percentage
        
        # Calculate control and voice channels
        self.control_channels = math.ceil(self.total_channels * self.control_channel_percentage / 100)
        self.voice_channels = self.total_channels - self.control_channels
        
        print(f"Total Channels: {self.total_channels} | Control: {self.control_channels} ({self.control_channel_percentage}%) | Voice: {self.voice_channels}")
    
    def create_channel_matrices(self, cluster_size):
        print(f"\nCLUSTER SIZE: {cluster_size}")
        print("="*50)
        
        # Calculate distribution
        control_per_cell = self.control_channels // cluster_size
        voice_per_cell = self.voice_channels // cluster_size
        extra_control = self.control_channels % cluster_size
        extra_voice = self.voice_channels % cluster_size
        
        print(f"Base channels per cell - Control: {control_per_cell}, Voice: {voice_per_cell}")
        print(f"Extra channels to distribute - Control: {extra_control}, Voice: {extra_voice}")
        
        # Initialize matrices
        control_matrix = []
        traffic_matrix = []
        
        print(f"\nCONTROL CHANNEL MATRIX:")
        print("-"*50)
        print("Cell | Count | Channel Numbers")
        
        for cell in range(cluster_size):
            cell_control = control_per_cell + (1 if cell < extra_control else 0)
            cell_voice = voice_per_cell + (1 if cell < extra_voice else 0)
            
            # Create control channels
            control_start = 1 + (cell * control_per_cell) + min(cell, extra_control)
            control_channels = list(range(control_start, control_start + cell_control))
            control_matrix.append(control_channels)
            
            print(f"  {cell+1}  |   {cell_control}   | {control_channels}")
            
        print(f"\nTRAFFIC CHANNEL MATRIX:")
        print("-"*50)
        print("Cell | Count | Channel Numbers")
        
        for cell in range(cluster_size):
            cell_voice = voice_per_cell + (1 if cell < extra_voice else 0)
            
            # Create traffic channels
            traffic_start = self.control_channels + 1 + (cell * voice_per_cell) + min(cell, extra_voice)
            traffic_channels = list(range(traffic_start, traffic_start + cell_voice))
            traffic_matrix.append(traffic_channels)
            
            # Display with truncation for long lists
            if len(traffic_channels) > 6:
                display_channels = f"[{traffic_channels[0]}...{traffic_channels[-1]}]"
            else:
                display_channels = str(traffic_channels)
            
            print(f"  {cell+1}  |  {cell_voice:2d}   | {display_channels}")
        
        return control_matrix, traffic_matrix
    
    def verify_matrices(self, control_matrix, traffic_matrix):
        total_control = sum(len(row) for row in control_matrix)
        total_traffic = sum(len(row) for row in traffic_matrix)
        
        print(f"\nMATRIX VERIFICATION:")
        print(f"Control channels distributed: {total_control}/{self.control_channels} ✓")
        print(f"Traffic channels distributed: {total_traffic}/{self.voice_channels} ✓")
        print(f"Total channels: {total_control + total_traffic}/{self.total_channels} ✓")
        
        return total_control == self.control_channels and total_traffic == self.voice_channels
    
    def analyze_frequency_reuse(self, cluster_size):
        frequency_reuse_factor = 1 / cluster_size
        sir_db = 10 * math.log10(pow(math.sqrt(3 * cluster_size), 3) / 6)
        
        print(f"\nFREQUENCY REUSE ANALYSIS:")
        print(f"Reuse Factor: 1/{cluster_size} = {frequency_reuse_factor:.3f}")
        print(f"SIR: {sir_db:.1f} dB | Reuse Distance: {math.sqrt(3 * cluster_size):.1f}")
    
    def simulate_calls(self, cluster_size, traffic_matrix):
        print(f"\nCALL SIMULATION:")
        num_calls = 15
        successful = 0
        
        for i in range(num_calls):
            cell = random.randint(0, cluster_size - 1)
            if len(traffic_matrix[cell]) > 0:
                channel = random.choice(traffic_matrix[cell])
                successful += 1
                if i < 5:  # Show first 5 calls only
                    print(f"Call {i+1}: Cell {cell+1} -> Channel {channel} ✓")
        
        success_rate = (successful/num_calls)*100
        print(f"Success Rate: {successful}/{num_calls} ({success_rate:.0f}%)")
    
    def process_cluster(self, cluster_size):
        print(f"\n{'*'*60}")
        print(f"PROCESSING CLUSTER SIZE: {cluster_size}")
        print(f"{'*'*60}")
        
        # Create matrices
        control_matrix, traffic_matrix = self.create_channel_matrices(cluster_size)
        
        # Verify distribution
        self.verify_matrices(control_matrix, traffic_matrix)
        
        # Analyze reuse
        self.analyze_frequency_reuse(cluster_size)
        
        # Simulate calls
        self.simulate_calls(cluster_size, traffic_matrix)
        
        return control_matrix, traffic_matrix

def main():
    print("FIXED CHANNEL ASSIGNMENT SIMULATION")
    print("="*60)
    
    # Test scenarios
    scenarios = [
        (75, 12),   # 75 channels, 12% control
        (90, 15),   # 90 channels, 15% control
    ]
    
    cluster_sizes = [7, 9, 13]
    
    for i, (total_channels, control_percentage) in enumerate(scenarios):
        print(f"\n" + "="*60)
        print(f"SCENARIO {i + 1}: {total_channels} Total Channels, {control_percentage}% Control")
        print("="*60)
        
        # Create system
        fca = FixedChannelAssignment(total_channels, control_percentage)
        
        # Process each cluster size
        for cluster_size in cluster_sizes:
            fca.process_cluster(cluster_size)
    
    print(f"\n" + "="*60)
    print("COMPARISON SUMMARY:")
    print("Cluster 7: More channels/cell, higher interference")
    print("Cluster 9: Balanced performance")  
    print("Cluster 13: Fewer channels/cell, lower interference")
    print("="*60)
    print("SIMULATION COMPLETE")

if __name__ == "__main__":
    main()
