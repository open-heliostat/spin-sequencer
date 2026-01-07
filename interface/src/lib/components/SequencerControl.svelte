<script lang="ts">
	import Button from "$lib/components/Button.svelte";
	import GridForm from "$lib/components/GridForm.svelte";
	import SequencerProgressBar from "$lib/components/SequencerProgressBar.svelte";
	import SettingsCard from "$lib/components/SettingsCard.svelte";
	import { notifications } from "$lib/components/toasts/notifications";
	import { getJsonRest, postJsonRest } from "$lib/stores/rest";
	import type { SequencerStatus } from "$lib/types/models";
	import { onMount } from "svelte";

	let sequencerStatus: SequencerStatus;

    async function getSequencerStatus() {
        return getJsonRest("/rest/spin-seq/sequencer/status", sequencerStatus)
            .then((data) => {
                sequencerStatus = data;
            });
    }

	async function startSequencer() {
        return postJsonRest("/rest/spin-seq/sequencer", {control:{execute:1,run:true}}).then(() => {
            notifications.success("Sequencer started successfully.", 2000);
        }).catch((error) => {
            notifications.error("Failed to start sequencer: " + error.message, 3000);
        });
	}

	async function stopSequencer() {
		return postJsonRest("/rest/spin-seq/sequencer", {control:{execute:0,run:false}}).then(() => {
            notifications.success("Sequencer stopped successfully.", 2000);
        }).catch((error) => {
            notifications.error("Failed to stop sequencer: " + error.message, 3000);
        });
	}

	onMount(() => {
		getSequencerStatus();
		const interval = setInterval(getSequencerStatus, 1000);
		return () => clearInterval(interval);
	});

</script>

{#if sequencerStatus}
	<SettingsCard>
		{#snippet title()}
			<span class={sequencerStatus?.isRunning ? "text-green-500" : "text-red-500"}>Installation {sequencerStatus?.isRunning ? "Running" : "Stopped"}</span>
		{/snippet}
		<GridForm>
			<SequencerProgressBar
				{sequencerStatus}
			/>
		</GridForm>
		<Button
			label="Start"
			onClick={startSequencer}
			disabled={sequencerStatus.isRunning}
		/>
		<Button
			label="Stop"
			onClick={stopSequencer}
		/>
	</SettingsCard>
{/if}