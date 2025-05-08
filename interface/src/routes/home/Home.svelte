<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import StepperRestComp from '$lib/components/StepperRestComp.svelte';
	import CanConsoleRestComp from '$lib/components/CANConsoleRestComp.svelte';
	import CanSettingsRestComp from '$lib/components/CANSettingsRestComp.svelte';
	import SequencerRestComp from '$lib/components/SequencerRestComp.svelte';
	import { getJsonRest } from '$lib/stores/rest';
	import StepperStatusComp from '$lib/components/StepperStatusComp.svelte';
	import Spinner from '$lib/components/Spinner.svelte';
	import type { SpinDiagnostics } from '$lib/types/models';

	onMount(() => {
	});

	onDestroy(() => {
	});

	let restPath = 'rest/spin-seq/diag';

	let diag: SpinDiagnostics = {
		mcu: {
			version: '0.0.0',
			freeHeap: 0,
			freeSketchSpace: 0,	
		},
		stepper: {
			isEnabled: false,
			version: 0,
			status: 0
		},
		wifi: {
			apEnabled: 0,
			rssi: 0,
			ssid: '',
			ip: '',
		},
		encoder: {},
		can: {
			enabled: false,
			rxId: 0,
			speed: 0,
			messagePack: false,
		},
	};

	async function getDiag() {
		return getJsonRest(restPath, diag).then((data) => {
			diag = data;
			return diag;
		});
	}

</script>

<SettingsCard>
	<span slot="title">Status</span>
	{#await getDiag()}
	<Spinner></Spinner>
	{:then diag}
		<div class="grid grid-cols-1 md:grid-cols-2 gap-4">
			<!-- MCU Section -->
			<div class="bg-gray-50 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2">MCU</h3>
				<div class="space-y-2">
					<p class="text-sm">Version: <span class="font-mono">{diag.mcu.version}</span></p>
					<p class="text-sm">Free Heap: <span class="font-mono">{diag.mcu.freeHeap/1000} kB</span></p>
					<p class="text-sm">Free Sketch Space: <span class="font-mono">{diag.mcu.freeSketchSpace/1000} kB</span></p>
				</div>
			</div>

			<!-- Stepper Section -->
			<div class="bg-gray-50 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2">Stepper</h3>
				<div class="space-y-2">
					<p class="text-sm">Status: <span class="font-mono">{diag.stepper.status}</span></p>
					<p class="text-sm">Version: <span class="font-mono">{diag.stepper.version}</span></p>
					<p class="text-sm">
						Enabled: 
						<span class={diag.stepper.isEnabled ? "text-green-600" : "text-red-600"}>
							{diag.stepper.isEnabled ? "Yes" : "No"}
						</span>
					</p>
				</div>
			</div>

			<!-- WiFi Section -->
			<div class="bg-gray-50 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2">WiFi</h3>
				<div class="space-y-2">
					<p class="text-sm">SSID: <span class="font-mono">{diag.wifi.ssid || 'Not Connected'}</span></p>
					<p class="text-sm">IP: <span class="font-mono">{diag.wifi.ip || 'Not Available'}</span></p>
					<p class="text-sm">AP Enabled: <span class="font-mono">{diag.wifi.apEnabled ? 'Yes' : 'No'}</span></p>
					<p class="text-sm">RSSI: <span class="font-mono">{diag.wifi.rssi} dBm</span></p>
				</div>
			</div>

			<!-- Encoder Section -->
			<div class="bg-gray-50 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2">Encoder</h3>
				<div class="space-y-2">
					{#if Object.keys(diag.encoder).length === 0}
						<p class="text-sm text-gray-500 italic">No encoder data available</p>
					{:else}
						{#each Object.entries(diag.encoder) as [key, value]}
							<p class="text-sm">{key}: <span class="font-mono">{value}</span></p>
						{/each}
					{/if}
				</div>
			</div>

			<!-- CAN Section -->
			<div class="bg-gray-50 rounded-lg p-4">
				<h3 class="text-lg font-semibold mb-2">CAN</h3>
				<div class="space-y-2">
					<p class="text-sm">Enabled: <span class="font-mono">{diag.can.enabled ? 'Yes' : 'No'}</span></p>
					<p class="text-sm">RX ID: <span class="font-mono">{diag.can.rxId}</span></p>
					<p class="text-sm">Speed: <span class="font-mono">{diag.can.speed} kBps</span></p>
					<p class="text-sm">Messagepack: <span class="font-mono">{diag.can.messagePack ? 'Yes' : 'No'}</span></p>
				</div>
			</div>
		</div>
	{/await}
</SettingsCard>