<script lang="ts">
    import { onMount, onDestroy } from 'svelte';
    import SettingsCard from './SettingsCard.svelte';
    import GridForm from '$lib/components/GridForm.svelte';
    import Collapsible from '$lib/components/Collapsible.svelte';
    import { getJsonRest, postJsonRest } from '$lib/stores/rest';
    import { notifications } from "$lib/components/toasts/notifications";
    import Text from './Text.svelte';
    import Slider from './Slider.svelte';
    import Button from './Button.svelte';
    import Checkbox from './Checkbox.svelte';
    import CANTopology from '~icons/tabler/topology-bus'
	import { get } from 'svelte/store';
	import Spinner from './Spinner.svelte';

    export let restPath: string;

    type CanSettings = {
        enable: boolean;
        txId: number;
        rxId: number;
        messageHistory: string[];
        sendMessage?: string;
    };
    
    let canSettings: CanSettings = {
        enable: false,
        txId: 0,
        rxId: 0,
        messageHistory: []
    };

    async function getCanSettings() {
        return getJsonRest(restPath, canSettings).then((data) => {
            canSettings = data;
        });
    }
    async function postCanSettings() {
        return postJsonRest(restPath, canSettings).then((data) => {
            canSettings = data;
        });
    }
    getCanSettings();

</script>

<SettingsCard>
	<CANTopology slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
    <span slot="title">Can Settings</span>
    {#await getCanSettings()}
    <Spinner></Spinner>
    {:then nothing}
    <GridForm>
        <Checkbox
            label="Enable"
            bind:value={canSettings.enable}
            onChange={() => {
                postJsonRest(restPath + "/enable", canSettings.enable);
            }}
        />
        <Slider
            label="Tx ID"
            min={0}
            max={255}
            step={1}
            bind:value={canSettings.txId}
            onChange={postCanSettings}
        />
        <Slider
            label="Rx ID"
            min={0}
            max={255}
            step={1}
            bind:value={canSettings.rxId}
            onChange={postCanSettings}
        />
    </GridForm>
    {/await}
</SettingsCard>